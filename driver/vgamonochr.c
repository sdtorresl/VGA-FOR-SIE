/*
 * Desarrollado por:
 * Nicolás David Arias Sosa 		- ndariass@unal.edu.co
 * Gustavo Adolfo Valbuena Reyes 	- gavalbuenar@unal.edu.co
 * Sergio Daniel Torres Linares 	- sdtorresl@unal.edu.co
 *
 * Pregrado en Ingeniería Electrónica
 * Universidad Nacional de Colombia - Sede Bogotá
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/interrupt.h>

#include <linux/io.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>

#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>


#define DRIVER_NAME  "vgamonochrfb"

#define REG_MAX				0x0

#define MEM_SIZE 			320*240		//tamaño en bytes del fb
#define DEFAULT_BITS_PP 	8			//bits requeridos para almacenar un pixel
#define FPGA_BASE			0x15000000	//dirección base de la memoria donde se almacenará el fb

struct vgamonochrfb {
	struct fb_info *fb;

	void *vidmem_virt;
	unsigned long vidmem_phys;
	unsigned char __iomem *ctrlbase;

	uint32_t pseudo_palette[16];
};

//definición del modo de video a utilizar
static char *mode_option = "320x240@60";

/*
	Modos de video posible. Únicamente son del interes, para este
	driver las posiciones 2,3,4 de cada elemento (frecuencia de 
	actualización, resolución x, resolución y). Los demás
	parámetros hacen referencia a tiempos de los sincronismos
	para el control de la pantalla, cuyo valor es fijo y se 
	define en el hw. 
*/
static const struct fb_videomode vgamonochrfb_modedb[] = {
	{ NULL, 60, 640, 480, 40000, 47, 16, 32, 30, 96, 2, 0,
		FB_VMODE_NONINTERLACED },
	{ NULL, 72, 800, 600, 20000, 80, 32, 23, 37, 128, 6, 0,
		FB_VMODE_NONINTERLACED },
	{ NULL, 60, 1024, 768, 15384, 168, 8, 29, 3, 144, 6, 0,
		FB_VMODE_NONINTERLACED },
	{ NULL, 60, 320, 240, 40000, 47, 8, 32, 30, 96, 2, 0,
		FB_VMODE_NONINTERLACED },
};


static const struct fb_fix_screeninfo vgamonochrfb_fix = {
	.id =		"VGAmonochrFB",
	.type =		FB_TYPE_PACKED_PIXELS,
	.visual =	FB_VISUAL_TRUECOLOR,
	.accel =	FB_ACCEL_NONE,
	.smem_len = 76800, //320*240
	.line_length = 320
};

ssize_t fb_sys_read(struct fb_info *info, char __user *buf, size_t count,
		    loff_t *ppos)
{
	/*unsigned long p = *ppos;
	void *src;
	int err = 0;
	unsigned long total_size;

	if (info->state != FBINFO_STATE_RUNNING)
		return -EPERM;

	total_size = info->screen_size;

	if (total_size == 0)
		total_size = info->fix.smem_len;

	if (p >= total_size)
		return 0;

	if (count >= total_size)
		count = total_size;

	if (count + p > total_size)
		count = total_size - p;

	src = (void __force *)(info->screen_base + p);

	if (info->fbops->fb_sync)
		info->fbops->fb_sync(info);

	if (copy_to_user(buf, src, count))
		err = -EFAULT;

	if  (!err)
		*ppos += count;*/

	return 0;
}
EXPORT_SYMBOL_GPL(fb_sys_read);

ssize_t fb_sys_write(struct fb_info *info, const char __user *buf,
		     size_t count, loff_t *ppos)
{
	unsigned long p = *ppos;
	unsigned char *dst;
	int err = 0;
	unsigned long total_size;

	unsigned int i;
	unsigned int dir;
	unsigned char dato;


	if (info->state != FBINFO_STATE_RUNNING)
		return -EPERM;

	total_size = info->screen_size;

	if (total_size == 0)
		total_size = info->fix.smem_len;

	if (p > total_size)
		return -EFBIG;

	if (count > total_size) {
		err = -EFBIG;
		count = total_size;
	}

	if (count + p > total_size) {
		if (!err)
			err = -ENOSPC;

		count = total_size - p;
	}
	
	dst = (unsigned char __force *) (info->screen_base);
	
	if (info->fbops->fb_sync)
		info->fbops->fb_sync(info);
		
	
	/*
		Escritura como tal en hw. Como fue mencionado en la 
		documentación del proyecto, no es posible definir directamente el dato en la 
		dirección deseada. Por cada caracter escrito, se hace una manipulación de la dirección 
		y el dato para poder direccionar la memoria de 17 bits en hw, considerando la
		siguiente convención:
		
		direccion_proc -> direccion_hw[9:0]
		dato_proc[7:1] -> direccion_hw[16:10]
		dato_proc[0]   -> dato_hw
		
		Nótese que las aplicaciones en espacio de usuario deben tener en cuenta la 
		consideración anterior, si trabajan directamente sobre la dirección del fb.
	*/
	for (i = 0x0; i < count;i++){
		dir = i & 0x3FF;					//10 LSB de la dirección que llegará a la memoria en hw
		dato = (unsigned char)buf[i] & 1;		//obtención del LSB de determinado caracter del buffer
		
		dato =( dato + ((i&0x1FC00)>>9) )&0xFF;		 //definición de los MSB de la dirección de la memoria 
													//en hw, junto al dato de 1 bit
		dst[dir] = dato;	
	}

	if  (!err)
		*ppos += count;

	return (err) ? err : count;
}
EXPORT_SYMBOL_GPL(fb_sys_write);

static unsigned int get_line_length(unsigned int xres_virtual, unsigned int bpp)
{
	unsigned int length;

	length = xres_virtual * bpp;
	length = (length + 31) & ~31;
	length >>= 3;
	return length;
}

static int vgamonochrfb_check_var(struct fb_var_screeninfo *var,
			 struct fb_info *info)
{
	u_long line_length;

	if (var->bits_per_pixel != 16)
		return -EINVAL;

	if (!var->xres)
		var->xres = 1;
	if (!var->yres)
		var->yres = 1;
	if (var->xres > var->xres_virtual)
		var->xres_virtual = var->xres;
	if (var->yres > var->yres_virtual)
		var->yres_virtual = var->yres;

	if (var->rotate) {
		dev_dbg(info->device, "Rotation is not supported\n");
		return -EINVAL;
	}

	line_length = get_line_length(var->xres_virtual, var->bits_per_pixel);
	if (line_length * var->yres_virtual > info->fix.smem_len) {
		dev_dbg(info->device, "Not enough memory\n");
		return -ENOMEM;
	}

	var->red.offset = 11;
	var->red.length = 5;
	var->green.offset = 5;
	var->green.length = 6;
	var->blue.offset = 0;
	var->blue.length = 5;
	var->transp.offset = 0;
	var->transp.length = 0;
	var->red.msb_right = 0;
	var->green.msb_right = 0;
	var->blue.msb_right = 0;
	var->transp.msb_right = 0;

	return 0;
}

static int vgamonochrfb_set_par(struct fb_info *info)
{
	info->fix.line_length = get_line_length(info->var.xres_virtual,
						info->var.bits_per_pixel);
	
	return 0;
}

/* Based on CNVT_TOHW macro from skeletonfb.c */
static inline uint32_t vgamonochrfb_convert_color_to_hw(unsigned int val,
	struct fb_bitfield *bf)
{
	return (((val << bf->length) + 0x7fff - val) >> 16) << bf->offset;
}


static int vgamonochrfb_setcolreg(unsigned int regno,
		unsigned int red, unsigned int green, unsigned int blue,
		unsigned int transp, struct fb_info *info)
{
	struct vgamonochrfb *vgamonochrfb = info->par;
	uint32_t color;

	if (regno >= 16)
		return -EINVAL;

	color = vgamonochrfb_convert_color_to_hw(red, &info->var.red);
	color |= vgamonochrfb_convert_color_to_hw(green, &info->var.green);
	color |= vgamonochrfb_convert_color_to_hw(blue, &info->var.blue);
	color |= vgamonochrfb_convert_color_to_hw(transp, &info->var.transp);

	vgamonochrfb->pseudo_palette[regno] = color;

	return 0;
}

#if 0
/* Based on bf54x-lq043fb.c */
static int vgamonochrfb_mmap(struct fb_info *info, struct vm_area_struct *vma)
{
	struct vgamonochrfb *vgamonochrfb = info->par;

	vma->vm_start = (unsigned int)vgamonochrfb->vidmem_virt;
	vma->vm_end = vma->vm_start + info->fix.smem_len;
	vma->vm_flags |= VM_MAYSHARE | VM_SHARED;

	return 0;
}
#endif

static struct fb_ops vgamonochrfb_ops = {
	.owner		= THIS_MODULE,
	.fb_read	= fb_sys_read,
	.fb_write	= fb_sys_write,
	.fb_check_var	= vgamonochrfb_check_var,
	.fb_set_par	= vgamonochrfb_set_par,
	.fb_setcolreg	= vgamonochrfb_setcolreg,
	.fb_fillrect	= sys_fillrect,
	.fb_copyarea	= sys_copyarea,
	.fb_imageblit	= sys_imageblit,
#if 0
	.fb_mmap	= vgamonochrfb_mmap,
#endif
};

/*
	Asignación de dirección física y virtual correspondiente
	a la memoria donde se va a almacenar el fb
*/
static void map_fb_memory(struct vgamonochrfb *vgamonochrfb){

	/*
		Definición de la dirección virtual del fb, a partir de la dirección
		física (que en nuestro caso es fija). Notese que a pesar de que el
		bus de direcciones del procesador es de 13 bits (8kB), el 
		tamaño de fb que maneja el driver es de 75 kB, considerando la 
		manipulación que se hace sobre la dirección y el dato antes de 
		hacer escrituras en hw.
	*/
	vgamonochrfb->vidmem_virt = ioremap(FPGA_BASE, MEM_SIZE);
	vgamonochrfb->vidmem_phys = FPGA_BASE;
}

/*
	Se borra toda la pantalla al cargar el módulo
*/
static void erase_screen(const void * vidmem_virt){
	unsigned int i;
	unsigned char dato;
	unsigned int dir;
	unsigned char *dst = (unsigned char __force *) (vidmem_virt);
	
	/*
		Para la operación de borrado, la dirección y el dato que se 
		escriben en hw se manipulan de manera similar a como se explicó
		previamente, en la función de escritura
	*/
	for (i = 0; i < MEM_SIZE; i++){
		dir = i & 0x3FF;
		dato =( 0 + ((i&0x1FC00)>>9) )&0xFF;
		
		dst[dir] = dato;
	}
}

static int __devinit vgamonochrfb_probe(struct platform_device *ofdev)
{
	struct fb_info *info;
	struct vgamonochrfb *vgamonochrfb;
	int ret;

	if (!MEM_SIZE) {
		dev_err(&ofdev->dev, "no video-mem-size property set\n");
		return -ENODEV;
	}
	
	info = framebuffer_alloc(sizeof(struct vgamonochrfb), &ofdev->dev);
	if (!info)
		return -ENOMEM;

	vgamonochrfb = info->par;
	vgamonochrfb->fb = info;

	//vgamonochrfb->ctrlbase = ioremap(res.start, REG_MAX);

	info->fbops = &vgamonochrfb_ops;

	/* Asignación de memoria física y virtual del framebuffer */
	map_fb_memory(vgamonochrfb);

	if (!vgamonochrfb->vidmem_virt) {
		dev_err(&ofdev->dev, "could not allocate framebuffer\n");
		ret = -ENOMEM;
		goto err_framebuffer_release;
	}
	
	//se borra la pantalla
	erase_screen(vgamonochrfb->vidmem_virt);
	
	info->screen_base = vgamonochrfb->vidmem_virt;
	info->fix = vgamonochrfb_fix;
	info->pseudo_palette = vgamonochrfb->pseudo_palette;
	info->flags = FBINFO_FLAG_DEFAULT;
	info->fix.smem_start = vgamonochrfb->vidmem_phys;
	info->fix.smem_len = MEM_SIZE;

	//elección entre los modos de video definidos en vgamonochrfb_modedb
	fb_find_mode(	&info->var, 
					info, 
					mode_option,
					vgamonochrfb_modedb, 
					ARRAY_SIZE(vgamonochrfb_modedb), 
					NULL, 
					DEFAULT_BITS_PP);		//definición de bits_per_pixel
	
	ret = fb_alloc_cmap(&info->cmap, 256, 0);

	
	if (ret < 0)
		goto err_dma_free;

	ret = register_framebuffer(info);

	if (ret < 0)
		goto err_dealloc_cmap;

	platform_set_drvdata(ofdev, vgamonochrfb);

	dev_info(&ofdev->dev,
	       "fb%d: VGAmonochr frame buffer at %p, size %ukB\n",
	       info->node, vgamonochrfb->vidmem_virt, MEM_SIZE >> 10);
	return 0;
	
	err_dealloc_cmap:
		fb_dealloc_cmap(&info->cmap);
	err_dma_free:
		vfree(vgamonochrfb->vidmem_virt);
	err_framebuffer_release:
		framebuffer_release(info);
	
	return ret;
}

static int __devexit vgamonochrfb_remove(struct platform_device *ofdev)
{
	struct vgamonochrfb *vgamonochrfb = platform_get_drvdata(ofdev);
	struct fb_info *info = vgamonochrfb->fb;

	unregister_framebuffer(info);

	fb_dealloc_cmap(&info->cmap);
	vfree(vgamonochrfb->vidmem_virt);
	framebuffer_release(info);

	platform_set_drvdata(ofdev, NULL);

	return 0;
}

static const struct of_device_id vgamonochr_vgafb_match[] = {
	{ .compatible = "vgamonochr,vgafb", },
	{},
};
MODULE_DEVICE_TABLE(of, vgamonochr_vgafb_match);

static struct platform_driver vgamonochr_vgafb_of_driver = {
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
		.of_match_table = vgamonochr_vgafb_match,
	},
	.probe		= vgamonochrfb_probe,
	.remove		= __devexit_p(vgamonochrfb_remove),
};

static struct platform_device *vga16fb_device;		//nuevo

#ifndef MODULE
static int __init vgamonochrfb_setup(char *options)
{
	char *this_opt;

	if (!options || !*options)
		return 0;

	while ((this_opt = strsep(&options, ",")) != NULL) {
		if (!*this_opt)
			continue;
		if (!strncmp(this_opt, "mode:", 5))
			mode_option = this_opt + 5;
		else
			printk(KERN_ERR "vgamonochr: unknown parameter %s\n",
					this_opt);
	}
	return 0;
}
#endif  /*  MODULE  */

static int __init vgamonochr_vgafb_init(void)
{
	int ret;			
#ifndef MODULE
	char *option = NULL;

	if (fb_get_options("vgamonochr", &option))
		return -ENODEV;
	vgamonochrfb_setup(option);
#endif

	ret = platform_driver_register(&vgamonochr_vgafb_of_driver);
	
	if (!ret) {
		vga16fb_device = platform_device_alloc("vgamonochrfb", 0);

		if (vga16fb_device) {
			/*	Se añade el dispositivo a la jerarquía de dispositivos.
			 	Con esto, se hace un llamado a la función probe (arriba)
			 	donde se crea el nodo /dev/fb1*/
			ret = platform_device_add(vga16fb_device);
		}
		else
			ret = -ENOMEM;

		if (ret) {
			platform_device_put(vga16fb_device);
			platform_driver_unregister(&vgamonochr_vgafb_of_driver);
		}
	}


	return ret;
}

static void __exit vgamonochr_vgafb_exit(void)
{
	platform_device_unregister(vga16fb_device);	
	platform_driver_unregister(&vgamonochr_vgafb_of_driver);
}

module_init(vgamonochr_vgafb_init);
module_exit(vgamonochr_vgafb_exit);

MODULE_AUTHOR("VGAmonochromatic Project");
MODULE_DESCRIPTION("VGAmonochromatic VGAFB driver");
MODULE_LICENSE("GPL");
