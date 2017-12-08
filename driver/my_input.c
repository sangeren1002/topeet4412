/* 参考drivers\input\keyboard\gpio_keys.c */

#include <linux/module.h>
#include <linux/version.h>

#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/irq.h>

#include <asm/gpio.h>
#include <asm/io.h>
#include <asm/arch/regs-gpio.h>


static struct input_dev *buttons_dev;
static struct pin_desc *irq_pd;
static struct timer_list buttons_timer;


struct pin_desc{
	int irq;
	char *name;
	unsigned int pin;
	unsigned int key_val;
};

struct pin_desc pins_desc[4] = {
	{IRQ_EINT0,  "S2", S3C2410_GPF0,   KEY_L},
	{IRQ_EINT2,  "S3", S3C2410_GPF2,   KEY_S},
	{IRQ_EINT11, "S4", S3C2410_GPG3,   KEY_ENTER},
	{IRQ_EINT19, "S5", S3C2410_GPG11,  KEY_LEFTSHIFT},
};


static irqreturn_t buttons_irq(unsigned int irq,void *dev_id)
{
	/* 10ms后启动定时器 */

	irq_pd = (struct pin_desc *)dev_id;
	
	mod_timer(&buttons_timer, jiffies+HZ/100);
	return IRQ_RETVAL(IRQ_HANDLED);
}
static void buttons_timer_function(unsigned int data)
{
	struct pin_desc *pindesc=irq_pd;
	unsigned int pinval;

	pinval = s3c2410_gpio_getpin(pindesc->pin);
	if(pinval)
	{
		input_event(buttons_dev, EV_KEY, pindesc->key_val, 0);
		input_sync(buttons_dev);
	}		
	else	
	{
		input_event(buttons_dev, EV_KEY, pindesc->key_val, 1);
		input_sync(buttons_dev);
	}
}


static int __init buttons_init(void)
{
	int i;
	/*分配dev结构体*/
	buttons_dev = input_allocate_device();

	/* 设置哪一类事件 */
	set_bit(EV_KEY,buttons_dev->evbit);//按键类
	set_bit(EV_REP, buttons_dev->evbit);//循环类

	/* 设置哪一个事件 */
	set_bit(KEY_L,buttons_dev->keybit);
	set_bit(KEY_S,buttons_dev->keybit);
	set_bit(KEY_ENTER,buttons_dev->keybit);
	set_bit(KEY_LEFTSHIFT,buttons_dev->keybit);

	input_register_device(buttons_dev);


	init_timer(&buttons_timer);
	buttons_timer.function = buttons_timer_function;
	add_timer(&buttons_timer);
	

	for(i=0; i<4; i++)
	{
		request_irq(pins_desc[i].irq,buttons_irq,IRQT_BOTHEDGE,pins_desc[i].name,&pins_desc[i]);
	}
	
	return 0;
}

static void __exit buttons_exit(void)
{
	int i;
	for(i=0; i<4; i++)
	{
		free_irq(pins_desc[i].irq,&pins_desc[i]);
	}
	del_timer(&buttons_timer);

	input_unregister_device(buttons_dev);
	input_free_device(buttons_dev);
}
module_init(buttons_init);

module_exit(buttons_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Phil Blundell <pb@handhelds.org>");
MODULE_DESCRIPTION("Keyboard driver for GPIOs");
MODULE_ALIAS("platform:gpio-keys");





