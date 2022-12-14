/*
 * Copyright (C) 2018 Hugh Cole-Baker
 *
 * Hugh Cole-Baker <sigmaris@gmail.com>
 * cpasjuste
 * digitalLumberjack <digitalLumberjack@recalbox.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>

#include <video/display_timing.h>
#include <video/of_display_timing.h>
#include <video/videomode.h>

#include <drm/drm_atomic_helper.h>
#include <drm/drm_bridge.h>
#include <drm/drm_crtc.h>
#include <drm/drm_crtc_helper.h>
#include <drm/drm_mipi_dsi.h>
#include <drm/drm_print.h>
#include <drm/drm_probe_helper.h>

// ~ 20 timings line + comments
#define READ_SIZE_MAX 2048
#define LINE_SIZE_MAX 256

static char read_buf[READ_SIZE_MAX];
static const char *timings_path = "/boot/crt/timings.txt";
static const char *config_path = "/boot/crt/recalbox-crt-options.cfg";

// Global config
static struct rrgbdualconfiguration {
  int voffset;
  int hoffset;
};


struct dpidac {
  struct drm_bridge bridge;
  struct drm_connector connector;
};

// Gpio status
static struct gpiodesc {
  struct gpio_desc *gpio;
  int gpio_state;
} dip50Hz, dip31kHz;

enum ModeIds {
  p320x240 = 0,
  p1920x240,
  p1920x224,
  p384x288,
  p1920x288,
  p1920x240at120,
  i768x576,
  i640x480,
  p640x480,
  ModeCount,
};

static const char* ModeNames[] = {
    "p320x240",
    "p1920x240",
    "p1920x224",
    "p384x288",
    "p1920x288",
    "p1920x240at120",
    "i768x576",
    "i640x480",
    "p640x480",
    "ModeCount",
};
static struct rrgbdualconfiguration modeconfigs[ModeCount] = {
    {.voffset = 0, .hoffset = 0},
    {.voffset = 0, .hoffset = 0},
    {.voffset = 0, .hoffset = 0},
    {.voffset = 0, .hoffset = 0},
    {.voffset = 0, .hoffset = 0},
    {.voffset = 0, .hoffset = 0},
    {.voffset = 0, .hoffset = 0},
    {.voffset = 0, .hoffset = 0},
    {.voffset = 0, .hoffset = 0},
};

static struct videomode modes[ModeCount] = {

    // 240p@60 : 320 1 4 30 46 240 1 4 5 14 0 0 0 60 0 6400000 1
    {
        .pixelclock = 6400000,
        .hactive = 320,
        .hfront_porch = 4,
        .hsync_len = 30,
        .hback_porch = 46,
        .vactive = 240,
        .vfront_porch = 4,
        .vsync_len = 5,
        .vback_porch = 14,
        .flags = DISPLAY_FLAGS_VSYNC_LOW | DISPLAY_FLAGS_HSYNC_LOW
    },
    // 1920x240p@60 : 1920 1 80 184 312 240 1 1 3 16 0 0 0 60 0 38937600 1
    {
        .pixelclock = 38937600,
        .hactive = 1920,
        .hfront_porch = 80,
        .hsync_len = 184,
        .hback_porch = 312,
        .vactive = 240,
        .vfront_porch = 1,
        .vsync_len = 3,
        .vback_porch = 16,
        .flags = DISPLAY_FLAGS_VSYNC_LOW | DISPLAY_FLAGS_HSYNC_LOW
    },
    // 1920x224p@60 : 1920 1 80 184 312 224 1 10 3 24 0 0 0 60 0 39087360 1
    {
        .pixelclock = 39087360,
        .hactive = 1920,
        .hfront_porch = 80,
        .hsync_len = 184,
        .hback_porch = 312,
        .vactive = 224,
        .vfront_porch = 10,
        .vsync_len = 3,
        .vback_porch = 24,
        .flags = DISPLAY_FLAGS_VSYNC_LOW | DISPLAY_FLAGS_HSYNC_LOW
    },
    // 288p@50 : 384 1 16 32 40 288 1 3 2 19 0 0 0 50 0 7363200
    {
        .pixelclock = 7363200,
        .hactive = 384,
        .hfront_porch = 16,
        .hsync_len = 32,
        .hback_porch = 40,
        .vactive = 288,
        .vfront_porch = 3,
        .vsync_len = 2,
        .vback_porch = 19,
        .flags = DISPLAY_FLAGS_VSYNC_LOW | DISPLAY_FLAGS_HSYNC_LOW
    },
    // 1920x288p@50 : 1920 1 80 184 312 288 1 4 3 18 0 0 0 50 0 39062400 1
    {
        .pixelclock = 39062400,
        .hactive = 1920,
        .hfront_porch = 80,
        .hsync_len = 184,
        .hback_porch = 312,
        .vactive = 288,
        .vfront_porch = 4,
        .vsync_len = 3,
        .vback_porch = 18,
        .flags = DISPLAY_FLAGS_VSYNC_LOW | DISPLAY_FLAGS_HSYNC_LOW
    },
    // 1920x240p@120 : 1920 1 48 208 256 240 1 4 3 15 0 0 0 120 0 76462080 1
    {
        .pixelclock = 76462080,
        .hactive = 1920,
        .hfront_porch = 48,
        .hsync_len = 208,
        .hback_porch = 256,
        .vactive = 240,
        .vfront_porch = 4,
        .vsync_len = 3,
        .vback_porch = 15,
        .flags = DISPLAY_FLAGS_VSYNC_LOW | DISPLAY_FLAGS_HSYNC_LOW
    },

    // 576i@50 : 768 1 24 72 88 576 1 6 5 38 0 0 0 50 1 14875000 1
    {
      .pixelclock = 14875000,
      .hactive = 768,
      .hfront_porch = 24,
      .hsync_len = 72,
      .hback_porch = 88,
      .vactive = 576,
      .vfront_porch = 6,
      .vsync_len = 5,
      .vback_porch = 38,
      .flags = DISPLAY_FLAGS_VSYNC_LOW | DISPLAY_FLAGS_HSYNC_LOW | DISPLAY_FLAGS_INTERLACED
    },

    // 480i@60 : 640 1 24 64 104 480 1 3 6 34 0 0 0 60 1 13054080 1
   {
      .pixelclock = 13054080,
      .hactive = 640,
      .hfront_porch = 24,
      .hsync_len = 64,
      .hback_porch = 104,
      .vactive = 480,
      .vfront_porch = 3,
      .vsync_len = 6,
      .vback_porch = 34,
      .flags = DISPLAY_FLAGS_VSYNC_LOW | DISPLAY_FLAGS_HSYNC_LOW | DISPLAY_FLAGS_INTERLACED
    },
    // 480p@60 : 640 1 24 96 48 480 1 11 2 32 0 0 0 60 0 25452000 1
    {
      .pixelclock = 25452000,
      .hactive = 640,
      .hfront_porch = 24,
      .hsync_len = 96,
      .hback_porch = 48,
      .vactive = 480,
      .vfront_porch = 11,
      .vsync_len = 2,
      .vback_porch = 32,
      .flags = DISPLAY_FLAGS_VSYNC_LOW | DISPLAY_FLAGS_HSYNC_LOW
    }
};


static void dpidac_offset_and_validate(struct videomode *vm, int hoffset, int voffset) {

  hoffset *= vm->hactive / 320;
  if ((int) vm->hfront_porch - hoffset >= 1) {
    vm->hfront_porch -= hoffset;
    vm->hback_porch += hoffset;
  } else {
    // minimum front porch = 1
    vm->hback_porch += vm->hfront_porch - 1;
    vm->hfront_porch = 1;
  }
  // Horribeul special case for 480i
  int min_voffset = 1;
  if (vm->pixelclock == 13054080) {
    min_voffset = 2;
  }
  if ((int) vm->vfront_porch - voffset >= min_voffset) {
    vm->vfront_porch -= voffset;
    vm->vback_porch += voffset;
  } else {
    vm->vback_porch += (vm->vfront_porch - min_voffset);
    vm->vfront_porch = min_voffset;
  }
  printk(KERN_INFO "[RECALBOXRGBDUAL]: modified mode %d - %d %d %d\n",
         vm->vactive,
         vm->vfront_porch,
         vm->vsync_len,
         vm->vback_porch);

}

static struct drm_display_mode *dpidac_display_mode_from_timings(struct drm_connector *connector, const char *line) {
  int ret, hsync, vsync, interlace, ratio;
  struct drm_display_mode *mode = NULL;
  struct videomode vm;

  if (line != NULL) {
    memset(&vm, 0, sizeof(vm));
    ret = sscanf(line, "%d %d %d %d %d %d %d %d %d %d %*s %*s %*s %*s %d %ld %d",
                 &vm.hactive, &hsync, &vm.hfront_porch, &vm.hsync_len, &vm.hback_porch,
                 &vm.vactive, &vsync, &vm.vfront_porch, &vm.vsync_len, &vm.vback_porch,
                 &interlace, &vm.pixelclock, &ratio);
    if (ret != 13) {
      printk(KERN_WARNING "[RECALBOXRGBDUAL]: malformed mode requested, skipping (%s)\n", line);
      return NULL;
    }

    // setup flags
    vm.flags = interlace ? DISPLAY_FLAGS_INTERLACED : 0;
    vm.flags |= hsync ? DISPLAY_FLAGS_HSYNC_LOW : DISPLAY_FLAGS_HSYNC_HIGH;
    vm.flags |= vsync ? DISPLAY_FLAGS_VSYNC_LOW : DISPLAY_FLAGS_VSYNC_HIGH;

    // create/init display mode, convert from video mode
    mode = drm_mode_create(connector->dev);
    if (mode == NULL) {
      printk(KERN_WARNING "[RECALBOXRGBDUAL]: drm_mode_create failed, skipping (%s)\n", line);
      return NULL;
    }

    dpidac_offset_and_validate(&vm, 0, 0);
    drm_display_mode_from_videomode(&vm, mode);

    return mode;
  }

  return NULL;
}

int dpidac_load_timings(struct drm_connector *connector) {
  struct file *fp = NULL;
  ssize_t read_size = 0;
  size_t cursor = 0;
  char line[LINE_SIZE_MAX];
  size_t line_start = 0;
  size_t line_len = 0;
  struct drm_display_mode *mode = NULL;
  int mode_count = 0;

  fp = filp_open(timings_path, O_RDONLY, 0);
  if (IS_ERR(fp) || !fp) {
    printk(KERN_WARNING "[RECALBOXRGBDUAL]: timings file not found, skipping custom modes loading\n");
    return 0;
  }

  read_size = kernel_read(fp, &read_buf, READ_SIZE_MAX, &fp->f_pos);
  if (read_size <= 0) {
    filp_close(fp, NULL);
    printk(KERN_WARNING "[RECALBOXRGBDUAL]: empty timings file found, skipping custom modes loading\n");
    return 0;
  }
  filp_close(fp, NULL);

  for (cursor = 0; cursor < read_size; cursor++) {
    line[cursor - line_start] = read_buf[cursor];
    line_len++;
    if (line_len >= LINE_SIZE_MAX || read_buf[cursor] == '\n' || read_buf[cursor] == '\0') {
      if (line_len > 32 && line[0] != '#') {
        line[line_len - 1] = '\0';
        if ((mode = dpidac_display_mode_from_timings(connector, line)) != NULL) {
          mode->type = mode_count ? DRM_MODE_TYPE_DRIVER : DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
          printk(KERN_INFO "[RECALBOXRGBDUAL]: \t" DRM_MODE_FMT, DRM_MODE_ARG(mode));

          drm_mode_probed_add(connector, mode);
          mode_count++;
        }
      }
      line_start += line_len;
      line_len = 0;
      memset(line, 0, 128);
    }
  }

  return mode_count;
}

static int dpidac_load_config(const char *configfile) {
  struct file *fp = NULL;
  ssize_t read_size = 0;
  size_t cursor = 0;
  char line[LINE_SIZE_MAX];
  char optionname[LINE_SIZE_MAX];
  char optionbuffer[LINE_SIZE_MAX];
  int optionvalue = 0;
  size_t line_start = 0;
  size_t line_len = 0;
  int scanret = 0;
  int modeId = 0;

  fp = filp_open(config_path, O_RDONLY, 0);
  if (IS_ERR(fp) || !fp) {
    printk(KERN_INFO "[RECALBOXRGBDUAL]: config file not found, skipping configuration loading\n");
    return 0;
  }

  read_size = kernel_read(fp, &read_buf, READ_SIZE_MAX, &fp->f_pos);
  if (read_size <= 0) {
    filp_close(fp, NULL);
    printk(KERN_INFO "[RECALBOXRGBDUAL]: empty config file found, skipping configuration loading\n");
    return 0;
  }
  filp_close(fp, NULL);
  for (cursor = 0; cursor < read_size; cursor++) {
    line[cursor - line_start] = read_buf[cursor];
    line_len++;
    if (line_len >= LINE_SIZE_MAX || read_buf[cursor] == '\n' || read_buf[cursor] == '\0') {
      if (line_len > 1 && line[0] != '#') {
        line[line_len - 1] = '\0';
        scanret = sscanf(line, "%s = %d", &optionname, &optionvalue);
        if (scanret == 2) {
          for(modeId = 0; modeId < ModeCount; modeId++){
            sprintf(optionbuffer, "mode.offset.%s.verticaloffset", ModeNames[modeId]);
            if (strcmp(optionname, optionbuffer) == 0) {
              printk(KERN_INFO "[RECALBOXRGBDUAL]: setting %s to %d\n", optionbuffer, optionvalue);
              modeconfigs[modeId].voffset = optionvalue;
            }
            sprintf(optionbuffer, "mode.offset.%s.horizontaloffset", ModeNames[modeId]);
            if (strcmp(optionname, optionbuffer) == 0) {
              printk(KERN_INFO "[RECALBOXRGBDUAL]: setting %s to %d\n", optionbuffer, optionvalue);
              modeconfigs[modeId].hoffset = optionvalue;
            }
          }

        }
      }
      line_start += line_len;
      line_len = 0;
      memset(line, 0, 128);
    }
  }
  return 0;
}


static inline struct dpidac *drm_bridge_to_dpidac(struct drm_bridge *bridge) {
  return container_of(bridge, struct dpidac, bridge);
}

static inline struct dpidac *drm_connector_to_dpidac(struct drm_connector *connector) {
  return container_of(connector, struct dpidac, connector);
}


static void dpidac_apply_module_mode(struct drm_connector *connector, int modeId, bool preferred) {
  struct drm_device *dev = connector->dev;
  struct drm_display_mode *mode = drm_mode_create(dev);
  struct videomode vmcopy;
  struct videomode *vm = &modes[modeId];
  vmcopy.vback_porch = vm->vback_porch;
  vmcopy.vfront_porch = vm->vfront_porch;
  vmcopy.hback_porch = vm->hback_porch;
  vmcopy.hfront_porch = vm->hfront_porch;
  vmcopy.flags = vm->flags;
  vmcopy.hactive = vm->hactive;
  vmcopy.hsync_len = vm->hsync_len;
  vmcopy.pixelclock = vm->pixelclock;
  vmcopy.vactive = vm->vactive;
  vmcopy.vsync_len = vm->vsync_len;

  dpidac_offset_and_validate(&vmcopy, modeconfigs[modeId].hoffset, modeconfigs[modeId].voffset);
  drm_display_mode_from_videomode(&vmcopy, mode);
  mode->type = DRM_MODE_TYPE_DRIVER;
  if (preferred)
    mode->type |= DRM_MODE_TYPE_PREFERRED;

  drm_mode_set_name(mode);
  drm_mode_probed_add(connector, mode);
}

static int dpidac_get_modes(struct drm_connector *connector) {
  int i;
  dpidac_load_config(config_path);
  i = dpidac_load_timings(connector);
  if (i) {
    printk(KERN_INFO "[RECALBOXRGBDUAL]: dpidac_get_modes: %i custom modes loaded\n", i);
    return i;
  } else {
    if (dip31kHz.gpio_state == 0) {
      printk(KERN_INFO "[RECALBOXRGBDUAL]: 31kHz modes will be available\n", i);
      dpidac_apply_module_mode(connector, p640x480, true);
      dpidac_apply_module_mode(connector, p1920x240at120, false);

      return 2;
    } else {
      if (dip50Hz.gpio_state == 0) {
        // 50hz
        printk(KERN_INFO "[RECALBOXRGBDUAL]: 50Hz modes will be available\n", i);
        dpidac_apply_module_mode(connector, p384x288, true);
        dpidac_apply_module_mode(connector, p1920x288, false);
        dpidac_apply_module_mode(connector, i768x576, false);
        return 3;
      } else {
        printk(KERN_INFO "[RECALBOXRGBDUAL]: 60Hz + 50Hz modes will be available\n");
        dpidac_apply_module_mode(connector, p320x240, true);
        dpidac_apply_module_mode(connector, p1920x240, false);
        dpidac_apply_module_mode(connector, p1920x224, false);
        dpidac_apply_module_mode(connector, p384x288, false);
        dpidac_apply_module_mode(connector, p1920x288, false);
        dpidac_apply_module_mode(connector, i640x480, false);
        dpidac_apply_module_mode(connector, i768x576, false);
        return 7;
      }
    }
  }
  return 1;
}

static const struct drm_connector_helper_funcs dpidac_con_helper_funcs = {
    .get_modes    = dpidac_get_modes,
};

static enum drm_connector_status dpidac_connector_detect(struct drm_connector *connector, bool force) {
  return connector_status_connected;
}

static const struct drm_connector_funcs dpidac_con_funcs = {
    .detect            = dpidac_connector_detect,
    .fill_modes        = drm_helper_probe_single_connector_modes,
    .destroy        = drm_connector_cleanup,
    .reset            = drm_atomic_helper_connector_reset,
    .atomic_duplicate_state    = drm_atomic_helper_connector_duplicate_state,
    .atomic_destroy_state    = drm_atomic_helper_connector_destroy_state,
};

static int dpidac_attach(struct drm_bridge *bridge, enum drm_bridge_attach_flags flags) {
  struct dpidac *vga = drm_bridge_to_dpidac(bridge);
  u32 bus_format = MEDIA_BUS_FMT_RGB666_1X18;
  u32 mode;
  int ret;

  if (!bridge->encoder) {
    DRM_ERROR("Missing encoder\n");
    return -ENODEV;
  }

  drm_connector_helper_add(&vga->connector,
                           &dpidac_con_helper_funcs);
  ret = drm_connector_init(bridge->dev, &vga->connector,
                           &dpidac_con_funcs, DRM_MODE_CONNECTOR_VGA);
  if (ret) {
    DRM_ERROR("Failed to initialize connector\n");
    return ret;
  }

  of_property_read_u32(vga->bridge.of_node, "vc4-vga666-mode", &mode);
  printk(KERN_INFO "[RECALBOXRGBDUAL]: vc4-vga666 mode: %i\n", mode);
  if (mode == 6) {
    bus_format = MEDIA_BUS_FMT_RGB666_1X24_CPADHI;
  }

  ret = drm_display_info_set_bus_formats(&vga->connector.display_info,
                                         &bus_format, 1);
  if (ret) {
    DRM_ERROR("Failed to set bus format\n");
    return ret;
  }

  vga->connector.interlace_allowed = 1;
  vga->connector.doublescan_allowed = 1;

  drm_connector_attach_encoder(&vga->connector,
                               bridge->encoder);

  return 0;
}

static const struct drm_bridge_funcs dpidac_bridge_funcs = {
    .attach        = dpidac_attach,
};


static int dpidac_probe(struct platform_device *pdev) {
  struct dpidac *vga;
  u32 rgbdual = 0;

  vga = devm_kzalloc(&pdev->dev, sizeof(*vga), GFP_KERNEL);
  if (!vga)
    return -ENOMEM;
  platform_set_drvdata(pdev, vga);

  vga->bridge.funcs = &dpidac_bridge_funcs;
  vga->bridge.of_node = pdev->dev.of_node;

  of_property_read_u32(vga->bridge.of_node, "recalbox-rgb-dual", &rgbdual);

  if (rgbdual == 1) {
    printk(KERN_INFO "[RECALBOXRGBDUAL]: Thank you for your support!\n");

    /* Switch 31kHz */
    dip31kHz.gpio = devm_gpiod_get_index(&(pdev->dev), "dipswitch", 0, GPIOD_IN);
    if (IS_ERR(dip31kHz.gpio)) {
      pr_err("Error when assigning GPIO.\n");
    }
    dip31kHz.gpio_state = gpiod_get_value(dip31kHz.gpio);
    gpiod_export(dip31kHz.gpio, false);
    gpiod_export_link(&pdev->dev, "dipswitch-31khz", dip31kHz.gpio);

    /* Switch 50 HZ */
    dip50Hz.gpio = devm_gpiod_get_index(&(pdev->dev), "dipswitch", 1, GPIOD_IN);
    if (IS_ERR(dip50Hz.gpio)) {
      pr_err("Error when assigning GPIO.\n");
    }
    dip50Hz.gpio_state = gpiod_get_value(dip50Hz.gpio);
    gpiod_export(dip50Hz.gpio, false);
    gpiod_export_link(&pdev->dev, "dipswitch-50hz", dip50Hz.gpio);

    printk(KERN_INFO "[RECALBOXRGBDUAL]: dip50Hz: %i, dip31kHz: %i\n", dip50Hz.gpio_state, dip31kHz.gpio_state);

  } else {
    dip50Hz.gpio_state = 1;
    dip31kHz.gpio_state = 1;
  }

  drm_bridge_add(&vga->bridge);

  return 0;
}

static int dpidac_remove(struct platform_device *pdev) {
  struct dpidac *vga = platform_get_drvdata(pdev);
  drm_bridge_remove(&vga->bridge);

  return 0;
}

static const struct of_device_id dpidac_match[] = {
    {.compatible = "raspberrypi,recalboxrgbdual"},
    {},
};
MODULE_DEVICE_TABLE(of, dpidac_match);

static struct platform_driver dpidac_driver = {
    .probe  = dpidac_probe,
    .remove = dpidac_remove,
    .driver = {
        .name        = "recalboxrgbdual",
        .of_match_table    = dpidac_match,
    },
};

module_platform_driver(dpidac_driver);

MODULE_AUTHOR("Hugh Cole-Baker and cpasjuste and digitalLumberjack");
MODULE_DESCRIPTION("Raspberry Pi Recalbox RGB Dual driver");
MODULE_LICENSE("GPL");
