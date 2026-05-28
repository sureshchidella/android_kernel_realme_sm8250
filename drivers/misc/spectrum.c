#include <linux/module.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>

static int spectrum_profile = 0;
// 0 = Balance, 1 = Performance, 2 = Battery, 3 = Gaming

static ssize_t profile_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", spectrum_profile);
}

static ssize_t profile_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	int ret;
	int profile;

	ret = kstrtoint(buf, 10, &profile);
	if (ret < 0)
		return ret;

	if (profile < 0 || profile > 3)
		return -EINVAL;

	spectrum_profile = profile;

	// Trigger a uevent for userspace to handle the actual frequency scaling
	kobject_uevent(kobj, KOBJ_CHANGE);

	return count;
}

static struct kobj_attribute profile_attribute =
	__ATTR(profile, 0666, profile_show, profile_store);

static struct attribute *spectrum_attrs[] = {
	&profile_attribute.attr,
	NULL,
};

static struct attribute_group spectrum_attr_group = {
	.attrs = spectrum_attrs,
};

static struct kobject *spectrum_kobj;

static int __init spectrum_init(void)
{
	int ret;

	spectrum_kobj = kobject_create_and_add("spectrum", kernel_kobj);
	if (!spectrum_kobj)
		return -ENOMEM;

	ret = sysfs_create_group(spectrum_kobj, &spectrum_attr_group);
	if (ret)
		kobject_put(spectrum_kobj);

	return ret;
}

static void __exit spectrum_exit(void)
{
	kobject_put(spectrum_kobj);
}

module_init(spectrum_init);
module_exit(spectrum_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Spectrum Kernel Profile Manager");
MODULE_AUTHOR("Antigravity");
