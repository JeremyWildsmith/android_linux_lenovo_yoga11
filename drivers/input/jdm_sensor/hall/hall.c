#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
//#include <linux/wakelock.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/of_device.h>
#include <linux/workqueue.h>
#include <linux/mutex.h>
#include <linux/regulator/consumer.h>
#include <linux/jiffies.h>
#include <linux/fb.h>

#define KEY_HALL_OPEN                0x289
#define KEY_HALL_CLOSE               0x290

//#define GPIO_HALL_EINT_PIN 107
#define CONFIG_HALL_SYS

static struct delayed_work hall_irq_event_work;
static struct workqueue_struct *hall_irq_event_wq;
static void hall_irq_event_workfunc(struct work_struct *work);
extern struct kobject *lenovo_keyboard_hall;
extern int kb_connect_status;

struct hall_switch_info
{
	struct mutex io_lock;
	u32 irq_gpio;
	u32 irq_gpio_flags;
	int irq;
	int hall_switch_state;
	struct input_dev *ipdev;
#ifdef CONFIG_HALL_SYS
	struct class *hall_sys_class;
#endif
};

struct hall_switch_info *global_hall_info;

static void hall_irq_event_workfunc(struct work_struct *work)
{
	int hall_gpio;
	char *envp[2];
	pr_err("Hall_sensor hall gpio state = %d\n",global_hall_info->hall_switch_state);
	hall_gpio = gpio_get_value_cansleep(global_hall_info->irq_gpio);
	pr_err("Hall_sensor hall irq interrupt gpio = %d\n", hall_gpio);
	if(hall_gpio == global_hall_info->hall_switch_state){
		enable_irq(global_hall_info->irq);
		return;
	}else{
		global_hall_info->hall_switch_state = hall_gpio;
		pr_err("Hall_sensor hall report key s ");
	}
	if (hall_gpio) {//open
			input_report_key(global_hall_info->ipdev, KEY_HALL_OPEN, 1);
			input_sync(global_hall_info->ipdev);
			input_report_key(global_hall_info->ipdev, KEY_HALL_OPEN, 0);
			input_sync(global_hall_info->ipdev);
			input_report_switch(global_hall_info->ipdev, 0, 0);
			input_sync(global_hall_info->ipdev);
			if (kb_connect_status) {
			    pr_err("lq_keyboard:%s:KEYBOARD_STATATUS=ENABLE;hall_gpio:%d  0:HALL_NEAR(CLOSE); 1:HALL_FAR(OPEN); kb_connect_status:%d\n", __func__, hall_gpio,kb_connect_status);
		        envp[0] = "KEYBOARD_STATATUS=ENABLE";
			    envp[1] = NULL;
			    kobject_uevent_env(lenovo_keyboard_hall, KOBJ_ONLINE, envp);
			    }
	}else{//close
			input_report_key(global_hall_info->ipdev, KEY_HALL_CLOSE, 1);
			input_sync(global_hall_info->ipdev);
			input_report_key(global_hall_info->ipdev, KEY_HALL_CLOSE, 0);
			input_sync(global_hall_info->ipdev);
			input_report_switch(global_hall_info->ipdev, 0, 1);
			input_sync(global_hall_info->ipdev);
			if (kb_connect_status) {
			pr_err("lq_keyboard:%s:KEYBOARD_STATATUS=DISABLE;hall_gpio:%d  0:HALL_NEAR(CLOSE); 1:HALL_FAR(OPEN); kb_connect_status:%d\n", __func__, hall_gpio,kb_connect_status);
		    envp[0] = "KEYBOARD_STATATUS=DISABLE";
			envp[1] = NULL;
			kobject_uevent_env(lenovo_keyboard_hall, KOBJ_OFFLINE, envp);
				}
	}
	enable_irq(global_hall_info->irq);
	pr_err("Hall_sensor hall en irq\n");
	return;
}

static irqreturn_t hall_interrupt(int irq, void *data)
{
	disable_irq_nosync(irq);
	queue_delayed_work(hall_irq_event_wq, &hall_irq_event_work,
			msecs_to_jiffies(150));
        return IRQ_HANDLED;
}

static int hall_parse_dt(struct device *dev, struct hall_switch_info *pdata)
{
	struct device_node *np = dev->of_node;

	pdata->irq_gpio = of_get_named_gpio_flags(np, "hall,irq-gpio",
				0, &pdata->irq_gpio_flags);
	if (pdata->irq_gpio < 0)
		return pdata->irq_gpio;

	pr_info("Hall_sensor hall irq_gpio=%d\n", pdata->irq_gpio);
	return 0;
}


static int hall_power_on(struct device *pdev)//for test mawenke
{
	int ret = 0;
	//struct regulator *hall_vdd;
	struct regulator *hall_vio;
	#if 0
	hall_vdd = regulator_get(pdev, "vdd");
	if (IS_ERR(hall_vdd)) {
		ret = -1;
		dev_err(pdev, "Regulator get failed vdd ret=%d\n", ret);
		return ret;
	}

	if (regulator_count_voltages(hall_vdd) > 0) {
		ret = regulator_set_voltage(hall_vdd, 2850000, 2850000);
		if (ret) {
 			dev_err(pdev, "Regulator set_vtg failed vdd ret=%d\n", ret);
			goto reg_vdd_put;
		}
	}

	ret = regulator_enable(hall_vdd);
	if (ret) {
		dev_err(pdev, "Regulator vdd enable failed ret=%d\n", ret);
		return ret;
	}
	#endif
	#if 1
	hall_vio = regulator_get(pdev, "vdd-io");
	if (IS_ERR(hall_vio)) {
		ret = -1;
		dev_err(pdev, "Regulator get failed vio ret=%d\n", ret);
		return ret;
	}

	if (regulator_count_voltages(hall_vio) > 0) {
		ret = regulator_set_voltage(hall_vio, 1800000, 1800000);
		if (ret) {
 			dev_err(pdev, "Regulator set_vtg failed vio ret=%d\n", ret);
			goto reg_vio_put;
		}
	}

	ret = regulator_enable(hall_vio);
	if (ret) {
		dev_err(pdev, "Regulator vio enable failed ret=%d\n", ret);
		return ret;
	}
	#endif
	
	return ret;

reg_vio_put:
	regulator_put(hall_vio);
//reg_vdd_put:
	//regulator_put(hall_vdd);
	return ret;
}

#ifdef CONFIG_HALL_SYS
static ssize_t hall_state_show(struct class *class,
		struct class_attribute *attr, char *buf)
{
	int state;
	if (global_hall_info->hall_switch_state) {
		state = 3;
	}else{
		state = 2;
	}
	pr_err("Hall_sensor hall_state_show state = %d, custome_state=%d\n", global_hall_info->hall_switch_state, state);
	return sprintf(buf, "%d\n", state);
}

static struct class_attribute hall_state =
	__ATTR(hall_state, S_IRUGO, hall_state_show, NULL);

static int hall_register_class_dev(struct hall_switch_info *hall_info){
	int err;
	if (!hall_info->hall_sys_class) {
		hall_info->hall_sys_class = class_create(THIS_MODULE, "hall_class");
		if (IS_ERR(hall_info->hall_sys_class)){
			hall_info->hall_sys_class = NULL;
			printk(KERN_ERR "could not allocate hall_class\n");
			return -1;
		}
	}

	err = class_create_file(hall_info->hall_sys_class, &hall_state);
	if(err < 0){
		class_destroy(hall_info->hall_sys_class);
		return -1;
	}
	return 0;
}
#endif

static int hall_probe(struct platform_device *pdev)
{
	int rc = 0;
	int err;
	struct hall_switch_info *hall_info;
	pr_info("Hall_sensor hall_probe\n");

	if (pdev->dev.of_node) {
		hall_info = kzalloc(sizeof(struct hall_switch_info), GFP_KERNEL);  
		if (!hall_info) {
			pr_err("Hall_sensor %s:hall failed to alloc memory for module data\n",__func__);
			return -ENOMEM;
		}
		err = hall_parse_dt(&pdev->dev, hall_info);
		if (err) {
			dev_err(&pdev->dev, "Hall_sensor hall_probe DT parsing failed\n");
			goto free_struct;
		}
	} else{
		return -ENOMEM;
	}
	mutex_init(&hall_info->io_lock);

	platform_set_drvdata(pdev, hall_info);

/*input system config*/
	hall_info->ipdev = input_allocate_device();
	if (!hall_info->ipdev) {
		pr_err("hall_probe: input_allocate_device fail\n");
		goto input_error;
	}
	hall_info->ipdev->name = "hall-switch-input";
	input_set_capability(hall_info->ipdev, EV_KEY, KEY_HALL_OPEN);
	input_set_capability(hall_info->ipdev, EV_KEY, KEY_HALL_CLOSE);
	input_set_capability(hall_info->ipdev, EV_SW, 0);
	//set_bit(INPUT_PROP_NO_DUMMY_RELEASE, hall_info->ipdev->propbit);
	rc = input_register_device(hall_info->ipdev);
	if (rc) {
		pr_err("hall_probe: input_register_device fail rc=%d\n", rc);
		goto input_error;
	}

	hall_power_on(&pdev->dev);


/*interrupt config*/
	if (gpio_is_valid(hall_info->irq_gpio)) {
		rc = gpio_request(hall_info->irq_gpio, "hall-switch-gpio");
		if (rc < 0) {
		        pr_err("hall_probe: gpio_request fail rc=%d\n", rc);
		        goto free_input_device;
		}

		rc = gpio_direction_input(hall_info->irq_gpio);
		if (rc < 0) {
		        pr_err("hall_probe: gpio_direction_input fail rc=%d\n", rc);
		        goto err_irq;
		}
		hall_info->hall_switch_state = gpio_get_value(hall_info->irq_gpio);
		pr_err("hall_probe: gpios = %d, gpion=%d\n", hall_info->hall_switch_state, hall_info->hall_switch_state);

		hall_info->irq = gpio_to_irq(hall_info->irq_gpio);
		pr_err("Hall_sensor hall irq = %d\n", hall_info->irq);

		rc = devm_request_threaded_irq(&pdev->dev, hall_info->irq, NULL,
			hall_interrupt,
			IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING|IRQF_ONESHOT, "hall-switch-irq", hall_info);
		if (rc < 0) {
			pr_err("hall_probe: request_irq fail rc=%d\n", rc);
			goto err_irq;
		}
		device_init_wakeup(&pdev->dev, true);
		irq_set_irq_wake(hall_info->irq,1);

	}else{
		pr_err("Hall_sensor hall irq gpio not provided\n");
	        goto free_input_device;
	}

	INIT_DELAYED_WORK(&hall_irq_event_work, hall_irq_event_workfunc);
	hall_irq_event_wq = create_workqueue("hall_irq_event_wq");
       pr_err("hall_probe end\n");
#ifdef CONFIG_HALL_SYS
	hall_register_class_dev(hall_info);
#endif
	   global_hall_info = hall_info;
       return 0;
err_irq:
	disable_irq_wake(hall_info->irq);
	device_init_wakeup(&pdev->dev, 0);
	gpio_free(hall_info->irq_gpio);

free_input_device:
	input_unregister_device(hall_info->ipdev);

input_error:
	platform_set_drvdata(pdev, NULL);
free_struct:
	kfree(hall_info);

	return rc;
}

static int hall_remove(struct platform_device *pdev)
{
	struct hall_switch_info *hall = platform_get_drvdata(pdev);
#ifdef CONFIG_HALL_SYS
	class_destroy(hall->hall_sys_class);
#endif
	pr_err("hall_remove\n");
	disable_irq_wake(hall->irq);
	device_init_wakeup(&pdev->dev, 0);
	free_irq(hall->irq, hall->ipdev);
	gpio_free(hall->irq_gpio);
	input_unregister_device(hall->ipdev);
	destroy_workqueue(hall_irq_event_wq);
	return 0;
}



static struct of_device_id sn_match_table[] = {
	{ .compatible = "p11,hall", },
	{ },
}; 

static struct platform_driver hall_driver = {
	.probe                = hall_probe,
	.remove                = hall_remove,
	.driver                = {
		.name        = "p11,hall",
		.owner        = THIS_MODULE,
		.of_match_table = sn_match_table,
	},
};

static int __init hall_init(void)
{
	pr_err("hall init enter\n");
	return platform_driver_register(&hall_driver);

}

static void __exit hall_exit(void)
{
	platform_driver_unregister(&hall_driver);
}


module_init(hall_init);
module_exit(hall_exit);
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("mawenke");