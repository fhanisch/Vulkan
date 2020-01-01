#include <stdio.h>
#include "libusb.h"

static void print_device(libusb_device* dev)
{
	int res;
	uint8_t path[8];
	libusb_device_handle *dev_handle;
	struct libusb_device_descriptor desc;
	unsigned char buf[1024];

	res = libusb_get_device_descriptor(dev, &desc);
	if (res < 0) {
		printf("Fuck!n");
		return;
	}
	printf("%04x:%04x (bus %d, device %d)",
		desc.idVendor, desc.idProduct,
		libusb_get_bus_number(dev), libusb_get_device_address(dev));
	res = libusb_get_port_numbers(dev, path, sizeof(path));
	if (res > 0) {
		printf(" path: %d", path[0]);
		for (int j = 1; j < res; j++)
			printf(".%d", path[j]);
	}
	res = libusb_open(dev, &dev_handle);
	//dev_handle = libusb_open_device_with_vid_pid(NULL, desc.idVendor, desc.idProduct);
	if (!res) {
		libusb_get_string_descriptor_ascii(dev_handle, desc.iManufacturer, buf, 1024);
		printf("\t%s: ", buf);
		libusb_get_string_descriptor_ascii(dev_handle, desc.iProduct, buf, 1024);
		printf("%s", buf);
	}
	else {
		printf("\tFuck!");
	}

	printf("\n");
		
}

void test_device(uint16_t vendor_id, uint16_t product_id)
{
	int res;
	int transferred;
	unsigned char buf[8];

	libusb_device_handle* hDev = libusb_open_device_with_vid_pid(NULL, vendor_id, product_id);
	if (!hDev) {
		printf("Scheisse!\n");
		return;
	}

	res = libusb_detach_kernel_driver(hDev, 0);
	if (res) {
		printf("Verdammt! (%d)\n", res);
		//return;
	}

	res = libusb_claim_interface(hDev, 0);
	if (res) {
		printf("Mist!\n");
		return;
	}

	while (1) {
		res = libusb_bulk_transfer(hDev, 0x81, buf, 8, &transferred, 0);
		
		if (res == LIBUSB_SUCCESS) {
			for (int i = 0; i < 8; i++) {
				printf("%3u ", buf[i]);
			}
			printf("\n");
		}
		else if (res == LIBUSB_ERROR_TIMEOUT)
		{
			printf("Timeout\n");
		}
		else {
			printf("Kacke! (%d)\n", res);
			return;
		}
	}

	res = libusb_attach_kernel_driver(hDev, 0);
}

int main(int argc, char** argv)
{
	libusb_device** devs;
	int res;
	ssize_t cnt;

	printf("\n*** USB ***\n\n");

	res = libusb_init(NULL);

	if (res) {
		printf("Fuck!\n");
		return res;
	}

	cnt = libusb_get_device_list(NULL, &devs);

	if (cnt < 0) {
		printf("Fuck!\n");
		libusb_exit(NULL);
		return (int)cnt;
	}

	printf("Number of devices: %d\n", (int) cnt);

	for (int i = 0; i < cnt; i++) {
		print_device(devs[i]);
	}

	libusb_free_device_list(devs, 1);

	test_device(0x046d, 0xc218);
	//test_device(0x046d, 0xc517);

	libusb_exit(NULL);

	return 0;
}