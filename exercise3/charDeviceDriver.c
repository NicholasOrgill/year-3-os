#include <linux/init.h>           // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <asm/uaccess.h>          // Required for the copy to user function
#include <linux/slab.h>
#include <linux/gfp.h>

#define  DEVICE_NAME "opsysmem"    ///< The device will appear at /dev/opsysmem using this value
#define  CLASS_NAME  "ops"        ///< The device class -- this is a character device driver

MODULE_LICENSE("GPL");            ///< The license type -- this affects available functionality
MODULE_AUTHOR("Nicholas Orgill");    ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("A character device implementing a simple method of message passing.");  ///< The description -- see modinfo
MODULE_VERSION("0.1");            ///< A version number to inform users

static int    majorNumber;                  ///< Stores the device number -- determined automatically
static int    numberOpens = 0;              ///< Counts the number of times the device is opened
static struct class *opsysmemClass  = NULL; ///< The device-driver class struct pointer
static struct device *opsysmemDevice = NULL; ///< The device-driver device struct pointer

typedef struct list {
	struct list_node* start;
	struct list_node* end;
	int size;
} list;

typedef struct list_node {
	char *message;
	int length;
	struct list_node* next;
} list_node;

static list *l;

int pop(void){
	int i;
	list_node *removing = l->start;

	// Size check
	if(l->size == 0)
		return -EAGAIN;

	// Put the message to the user
	for(i = 0; i < l->start->length; i++) {
		char c;
		put_user(c,&l->start->message[i]);
		if(c == '\0') break;
	}

	// Remove the node and update values
	l->start = removing->next;
	l->size -= removing->length;
	removing->message = NULL;
	removing->next = NULL;
	kfree(removing);
	return 0;
}

int push(const char *str, int len){
	list_node *node;

	// Total message size limit check
	if ((l->size + len) > 2097151)
		return -EAGAIN;

	// Message length check
	if (len > 4095)
		return -EINVAL;

	// Create new node and add to list
	node = kmalloc(sizeof(list_node), GFP_KERNEL);
	node->message = str;
	node->length = len;
	if (l->size == 0) {
		l->start = node;
	} else { 
		l->end->next = node;
		l->end = node;
	}
	l->size += len;
	return 0;
}

// The prototype functions for the character driver -- must come before the struct definition
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

/** @brief Devices are represented as file structure in the kernel. The file_operations structure from
 *  /linux/fs.h lists the callback functions that you wish to associated with your file operations
 *  using a C99 syntax structure. char devices usually implement open, read, write and release calls
 */
static struct file_operations fops =
{
   	.open = dev_open,
   	.read = dev_read,
   	.write = dev_write,
   	.release = dev_release,
};

/** @brief The LKM initialization function
 *  The static keyword restricts the visibility of the function to within this C file. The __init
 *  macro means that for a built-in driver (not a LKM) the function is only used at initialization
 *  time and that it can be discarded and its memory freed up after that point.
 *  @return returns 0 if successful
 */
static int __init opsysmem_init(void){
	l = kmalloc(sizeof(list), GFP_KERNEL);
   	printk(KERN_INFO "opsysmem: Initializing the opsysmem LKM\n");

   	// Try to dynamically allocate a major number for the device -- more difficult but worth it
   	majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   	if (majorNumber<0){
      	printk(KERN_ALERT "opsysmem failed to register a major number\n");
      	return majorNumber;
   	}
   	printk(KERN_INFO "opsysmem: registered correctly with major number %d\n", majorNumber);

   	// Register the device class
   	opsysmemClass = class_create(THIS_MODULE, CLASS_NAME);
   	if (IS_ERR(opsysmemClass)){                // Check for error and clean up if there is
      	unregister_chrdev(majorNumber, DEVICE_NAME);
      	printk(KERN_ALERT "Failed to register device class\n");
      	return PTR_ERR(opsysmemClass);          // Correct way to return an error on a pointer
   	}
   	printk(KERN_INFO "opsysmem: device class registered correctly\n");

   	// Register the device driver
   	opsysmemDevice = device_create(opsysmemClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   	if (IS_ERR(opsysmemDevice)){               // Clean up if there is an error
      	class_destroy(opsysmemClass);           // Repeated code but the alternative is goto statements
      	unregister_chrdev(majorNumber, DEVICE_NAME);
      	printk(KERN_ALERT "Failed to create the device\n");
      	return PTR_ERR(opsysmemDevice);
   	}
   	printk(KERN_INFO "opsysmem: device class created correctly\n"); // Made it! device was initialized
   	return 0;
}

/** @brief The LKM cleanup function
 *  Similar to the initialization function, it is static. The __exit macro notifies that if this
 *  code is used for a built-in driver (not a LKM) that this function is not required.
 */
static void __exit opsysmem_exit(void){
   	device_destroy(opsysmemClass, MKDEV(majorNumber, 0));     // remove the device
   	class_unregister(opsysmemClass);                          // unregister the device class
   	class_destroy(opsysmemClass);                             // remove the device class
   	unregister_chrdev(majorNumber, DEVICE_NAME);              // unregister the major number
   	printk(KERN_INFO "opsysmem: Goodbye from the LKM!\n");
}

/** @brief The device open function that is called each time the device is opened
 *  This will only increment the numberOpens counter in this case.
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_open(struct inode *inodep, struct file *filep){
   	numberOpens++;
   	printk(KERN_INFO "opsysmem: Device has been opened %d time(s)\n", numberOpens);
   	return 0;
}

/** @brief This function is called whenever device is being read from user space i.e. data is
 *  being sent from the device to the user. In this case is uses the copy_to_user() function to
 *  send the buffer string to the user and captures any errors.
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 *  @param buffer The pointer to the buffer to which this function writes the data
 *  @param len The length of the b
 *  @param offset The offset if required
 */
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){	
	pop();
	//buffer_size -= len;
	//buffer = krealloc(buffer, buffer_size, GFP_KERNEL);
	return len;
}

/** @brief This function is called whenever the device is being written to from user space i.e.
 *  data is sent to the device from the user. The data is copied to the message[] array in this
 *  LKM using the sprintf() function along with the length of the string.
 *  @param filep A pointer to a file object
 *  @param buffer The buffer to that contains the string to write to the device
 *  @param len The length of the array of data that is being passed in the const char buffer
 *  @param offset The offset if required
 */
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
	//int i, new_buffer_size;
	//char* krealloc_attempt;

	/*
	new_buffer_size = buffer_size + (sizeof(char)*len);
	if(new_buffer_size > maxSize)
		return -EAGAIN;	
	krealloc_attempt = krealloc(buffer, new_buffer_size, GFP_KERNEL);
	
	if(krealloc_attempt == NULL)
		return -EAGAIN; //failed to realloc
	
	buffer = krealloc_attempt;
	buffer_size = new_buffer_size;
	*/
	push(buffer, len);
	return len;
}

/** @brief The device release function that is called whenever the device is closed/released by
 *  the userspace program
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_release(struct inode *inodep, struct file *filep){
   	printk(KERN_INFO "opsysmem: Device successfully closed\n");
   	return 0;
}

/** @brief A module must use the module_init() module_exit() macros from linux/init.h, which
 *  identify the initialization function at insertion time and the cleanup function (as
 *  listed above)
 */
module_init(opsysmem_init);
module_exit(opsysmem_exit);
