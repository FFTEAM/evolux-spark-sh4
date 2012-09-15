/* 
 * e2_proc_misc.c
 */

#include <linux/proc_fs.h>  	/* proc fs */ 
#include <asm/uaccess.h>    	/* copy_from_user */

#include <linux/dvb/video.h>	/* Video Format etc */

#include <linux/dvb/audio.h>
#include <linux/smp_lock.h>
#include <linux/string.h>

#include "../backend.h"
#include "../dvb_module.h"
#include "linux/dvb/stm_ioctls.h"

extern struct DeviceContext_s* DeviceContext;

#if defined(IPBOX9900)

#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23)
#include <linux/stpio.h>
#else
#include <linux/stm/pio.h>
#endif

static int output_stat = 0;
struct stpio_pin *output_pin = NULL;

int proc_misc_12V_output_write(struct file *file, const char __user *buf,
                           unsigned long count, void *data)
{
	char 		*page = NULL;
	ssize_t 	ret = -ENOMEM;

//	printk("%s %d\n", __FUNCTION__, count);
	if (count > 0)
		page = (char *)__get_free_page(GFP_KERNEL);
	if (page) 
	{
		char *myString;
		ret = -EFAULT;
		if (copy_from_user(page, buf, count))
			goto out;

		myString = (char *) kmalloc(count + 1, GFP_KERNEL);
		strncpy(myString, page, count);
		myString[count] = '\0';
		printk("12V %s\n", myString);

                if(output_pin) {
			if (!strncmp("on", myString, 2)) //12V ON
				output_stat = 1;
			else if (!strncmp("off", myString, 3))
				output_stat = 0;

			stpio_set_pin(output_pin, output_stat);
		}
	
		kfree(myString);
	}

	ret = count;
out:
	if (page)
		free_page((unsigned long)page);
	return ret;
}


int proc_misc_12V_output_read (char *page, char **start, off_t off, int count,
			  int *eof, void *data_unused)
{
	int len = 0;
//	printk("%s %d %d\n", __FUNCTION__, count, current_input);

	if ( output_stat == 1  )
		len = sprintf(page, "on\n");
        else
		len = sprintf(page, "off\n");

        return len;
}

#endif
