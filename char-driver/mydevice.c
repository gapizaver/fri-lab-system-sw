#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/slab.h>
//#include <stdio.h>

#define DEVICE_NAME "mydevice"
// 10 za testiranje
#define BUF_SIZE 128

MODULE_LICENSE("GPL");

int zacni_modul(void);
void koncaj_modul(void);
int odpri(struct inode *, struct file *);
int sprosti(struct inode *, struct file *);
ssize_t beri(struct file *, char *, size_t, loff_t *);
ssize_t pisi(struct file *, const char *, size_t, loff_t *);

int counter = 0;
int Major;
struct file_operations fops = {
	.open = odpri,
	.release = sprosti,
	.read = beri,
	.write = pisi
};

module_init(zacni_modul);
module_exit(koncaj_modul);

int zacni_modul(void)
{
	Major = register_chrdev(0, DEVICE_NAME, &fops);
	if (Major < 0) {
		printk(KERN_ALERT "Registracija znakovne naprave spodletela.\n");
		return Major;
	}
	printk(KERN_INFO "Glavno stevilo je %d.\n", Major);
	return 0;
}

void koncaj_modul(void)
{
	unregister_chrdev(Major, DEVICE_NAME);
}

int odpri(struct inode *inode, struct file *file)
{ 
	counter++;
	return 0;
}

int sprosti(struct inode *inode, struct file *file)
{ 
	return 0; 
}

ssize_t beri(struct file *filp, char __user *buff, size_t len, loff_t *offset)
{
	char msg[BUF_SIZE];
	int size;
	
	// napiši sporočilo v buffer msg
	snprintf(msg, BUF_SIZE, "Odprt sem bil %d-krat\n", counter);
	size = strlen(msg);

	if ( *offset >= size)
		return 0;

	if ( len > size - *offset)
		// izračunaj koliko bytov vrniti
		len = size - *offset;

	if ( copy_to_user( buff, msg + *offset, len) )
		return -EFAULT;

	*offset += len;
	return len;
}

ssize_t pisi(struct file *filp, const char __user *buff, size_t len, loff_t *offset)
{
	return 0;
}
