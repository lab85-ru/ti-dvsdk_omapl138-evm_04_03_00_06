/*
 * Copyright (C) 2009 Texas Instruments Inc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option)any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/*
 * file   mcbsp.c
 * brief  McBSP Test Application
 *
 *   This kernel module tests the McBSP peripheral and the various features it
 *   supports. All the tests are done for DMA mode of operation
 *
 *   NOTE: THIS FILE IS PROVIDED ONLY FOR INITIAL DEMO RELEASE AND MAY BE
 *         REMOVED AFTER THE DEMO OR THE CONTENTS OF THIS FILE ARE SUBJECT
 *         TO CHANGE.
 *
 *   @author	Chaithrika U S
 *   @version	0.1 -
 *		Created on 10/06/09
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/moduleparam.h>
#include <linux/sysctl.h>
#include <linux/mm.h>
#include <linux/dma-mapping.h>
#include <linux/delay.h>

#include <mach/memory.h>
#include <mach/hardware.h>
#include <mach/irqs.h>
#include <mach/edma.h>
#include <mach/mcbsp.h>

static dma_addr_t physsrc = 0;
static char *bufsrc = NULL;
static dma_addr_t physdest = 0;
static char *bufdest = NULL;

static int device_id = 1;
static int buf_size = (1 * 1024);
static int buf_fmt = 0;
static int num_pkts = 1;
static int word_len = 32;
static int frame_len = 2;
static int freq = 48000;
static bool dlb = 0;
static int numevt = 0;
static int mc_mode = 0;
static int master = 1;
static int tx_rx = 3;

module_param(device_id, int, S_IRUGO);
module_param(buf_size, int, S_IRUGO);
module_param(buf_fmt, int, S_IRUGO);
module_param(num_pkts, int, S_IRUGO);
module_param(word_len, int, S_IRUGO);
module_param(frame_len, int, S_IRUGO);
module_param(freq, int, S_IRUGO);
module_param(dlb, bool, S_IRUGO);
module_param(numevt, int, S_IRUGO);
module_param(mc_mode, int, S_IRUGO);
module_param(master, int, S_IRUGO);
module_param(tx_rx, int, S_IRUGO);

MODULE_PARM_DESC(device_id, "McBSP instance (default:1)");
MODULE_PARM_DESC(buf_size, "Buffer Size (default:1KB)");
MODULE_PARM_DESC(buf_fmt, "Buffer format (default:Interleaved)");
MODULE_PARM_DESC(buf_size, "Packet count (default:1)");
MODULE_PARM_DESC(word_len, "Word Length: 8, 16, 24, 32 bits (default:32)");
MODULE_PARM_DESC(frame_len, "Frame Length: 1-128 (default:2)");
MODULE_PARM_DESC(freq, "Sample Frequency (default:48kHz)");
MODULE_PARM_DESC(dlb, "Digital Loopback (default: disabled)");
MODULE_PARM_DESC(numevt, "NUMEVT (default: 0)");
MODULE_PARM_DESC(mc_mode, "Multi-channel mode (default: 0)");
MODULE_PARM_DESC(master, "Master/Slave mode (default: master)");
MODULE_PARM_DESC(tx_rx, "Transmit, Receive or both (default: both)");

static int __init mcbsp_test_init(void)
{
	struct davinci_mcbsp_dev *dev = NULL;
	int i;
	int loopcnt = 0;
	int errcnt;

	if (tx_rx & 0x01) {
		bufsrc = dma_alloc_coherent(NULL, buf_size, &physsrc, 0);
		if (!bufsrc) {
			printk ("dma_alloc_coherent failed for physsrc\n");
			return -ENOMEM;
		}

		for (i = 0; i < buf_size; i++)
			bufsrc[i] = i % 256;
	}

	if (tx_rx & 0x02) {
		bufdest = dma_alloc_coherent(NULL, buf_size, &physdest, 0);
		if (!bufdest) {
			printk ("dma_alloc_coherent failed for physdest\n");
			return -ENOMEM;
		}
	}

	printk("Starting McBSP test\n");
	davinci_mcbsp_request(device_id, &dev);
	do {
		/* configure rcr and xcr */
		dev->tx_params.word_length1 = word_len; /* 32 bits */
		dev->tx_params.frame_length1 = frame_len;

		dev->rx_params.word_length1 = word_len; /* 32 bits */
		dev->rx_params.frame_length1 = frame_len;

		dev->rx_params.buf_fmt = buf_fmt;
		dev->tx_params.buf_fmt = buf_fmt;

		if (mc_mode > 0  && mc_mode < 8) {
			dev->rx_params.intr_mode = 1;
			dev->tx_params.intr_mode = 1;
		}

		davinci_mcbsp_config_params(dev, master);

		/* sample rate generator conifguration */
		davinci_mcbsp_config_clock(dev, freq);

		dev->tx_params.numevt = numevt;
		dev->rx_params.numevt = numevt;
		dev->tx_params.numdma = 1;
		dev->rx_params.numdma = 1;
		davinci_mcbsp_config_fifo(dev);


		dev->tx_params.mc_mode = mc_mode;
		dev->rx_params.mc_mode = mc_mode;

		dev->tx_params.cer[0] = 0x5555;
		dev->tx_params.cer[1] = 0x5555;
		dev->tx_params.cer[2] = 0x5555;
		dev->tx_params.cer[3] = 0x5555;
		dev->tx_params.cer[4] = 0x5555;
		dev->tx_params.cer[5] = 0x5555;
		dev->tx_params.cer[6] = 0x5555;
		dev->tx_params.cer[7] = 0x5555;

		dev->rx_params.cer[0] = 0x5555;
		dev->rx_params.cer[1] = 0x5555;
		dev->rx_params.cer[2] = 0x5555;
		dev->rx_params.cer[3] = 0x5555;
		dev->rx_params.cer[4] = 0x5555;
		dev->rx_params.cer[5] = 0x5555;
		dev->rx_params.cer[6] = 0x5555;
		dev->rx_params.cer[7] = 0x5555;

		davinci_mcbsp_config_multichannel_mode(dev);

		/* DMA Mode */
		dev->op_mode = DAVINCI_MCBSP_DMA_MODE;
		if (dlb) {
			printk("Setting DLB mode \n");
			dev->op_mode |= DAVINCI_MCBSP_DLB_MODE;
		} else {
			printk("No DLB mode \n");
		}

		if (tx_rx & 0x01) {
			printk("Transmitting Packet %d\n", loopcnt + 1);
			for (i = 0; i < buf_size; i++)
				bufsrc[i] = (i + loopcnt) % 256;

			printk("Starting McBSP TX\n");
			davinci_mcbsp_start_tx(dev);
		}

		if (tx_rx & 0x02) {
			printk("Starting McBSP RX\n");
			davinci_mcbsp_start_rx(dev);
		}

		printk("McBSP started\n");
		if (tx_rx & 0x02) {
			printk("Receving data...\n");
			davinci_mcbsp_recv_buf(dev, physdest, buf_size);
		}

		if (tx_rx & 0x01) {
			printk("Transmitting data...\n");
			davinci_mcbsp_xmit_buf(dev, physsrc, buf_size);
		}

		if (dev->op_mode & DAVINCI_MCBSP_DLB_MODE) {
			wait_for_completion(&dev->tx_params.dma_completion);
			printk("...TX complete\n");
			wait_for_completion(&dev->rx_params.dma_completion);
			printk("...RX complete\n");
		}

		if (tx_rx & 0x01) {
			davinci_mcbsp_stop_tx(dev);
			printk("TX stopped\n");
		}

		if (tx_rx & 0x02) {
			davinci_mcbsp_stop_rx(dev);
			printk("RX stopped\n");
		}

		if (tx_rx & 0x02 || (dev->op_mode & DAVINCI_MCBSP_DLB_MODE)) {
			printk("Testing data integrity for Packet %d: ", loopcnt + 1);
			errcnt = 0;
			for (i = 0; i < buf_size; i++) {
				if (bufdest[i] != ((i + loopcnt) % 256)) {
					errcnt = i + 1;
					break;
				}
			}

			if (errcnt) {
#if 1 /* debug */
				for (i = 0; i < buf_size; i++) {
					if (!(i % 10))
						printk("\nbufdest[%d]: ", i);
					printk("%d\t", bufdest[i]);
				}
				printk("\n");
#endif
				printk("Error found at Byte %d location\n", errcnt);
				davinci_mcbsp_dump_reg(dev);

			} else
				printk("Passed\n");

			memset(bufdest, 0 , buf_size);
		} else {
			mdelay(100);
			/* more time to compare data for large buffers */
			if (buf_size > 16 * 1024)
				mdelay(1000);
			if (buf_size >= 32 * 1024)
				mdelay(1000);
		}

		printk("-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.");
		printk("-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.\n");

	} while (++loopcnt < num_pkts);

#if 0	/* debug */
	davinci_mcbsp_dump_reg(dev);
#endif
	return 0;
}

static void mcbsp_test_exit(void)
{
	davinci_mcbsp_free(device_id);
	if (tx_rx & 0x01)
		dma_free_coherent(NULL, buf_size, bufsrc, physsrc);

	if (tx_rx & 0x02)
		dma_free_coherent(NULL, buf_size , bufdest, physdest);

	printk("McBSP test done... exiting\n");
}
module_init(mcbsp_test_init);
module_exit(mcbsp_test_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_LICENSE("GPL");
