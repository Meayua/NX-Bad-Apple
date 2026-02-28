/**
 * Nintendo Switch Homebrew Template
 * Target: libnx (Nintendo Switch Development Library) + pl_mpeg
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include switch development headers
#include <switch.h>

// Include pl_mpeg implementation
#define PL_MPEG_IMPLEMENTATION
#include "pl_mpeg.h"

// Global state
const char *video_location = "romfs:/video.mpg";
Framebuffer fb;
bool fb_initialized = false;
plm_t *plm = NULL;

/**
 * Callback function called by pl_mpeg whenever a new video frame is decoded.
 */
void on_video_callback(plm_t *self, plm_frame_t *frame, void *user) {
    if (!fb_initialized) return;

    u32 stride;
    // Get the pointer for the current backbuffer
    u32 *fb_ptr = (u32*)framebufferBegin(&fb, &stride);

    if (fb_ptr) {
        // Convert YUV to RGBA directly into the framebuffer
        plm_frame_to_rgba(frame, (uint8_t*)fb_ptr, stride * 4);
        
        // Finalize this frame immediately for the hardware
        framebufferEnd(&fb);
    }
}

int main(int argc, char **argv)
{
    // Initialize romfs
    Result rc = romfsInit();
    if (R_FAILED(rc)) {
        consoleInit(NULL);
        printf("Failed to initialize romfs: 0x%x\n", rc);
        return 1;
    }
    
    // Initialize pl_mpeg
    plm = plm_create_with_filename(video_location);

    if (!plm) {
        // Only init console if the video fails
        consoleInit(NULL);
        printf("Failed to open: %s\n", video_location);
    } else {
        plm_set_video_decode_callback(plm, on_video_callback, NULL);
        
        // Get the actual video properties (480x360) and mutliply height by 4 because uhh it cuts off if you don't idk why
        int width = plm_get_width(plm);
        int height = plm_get_height(plm)*4;

        framebufferCreate(&fb, nwindowGetDefault(), width, height, PIXEL_FORMAT_RGBA_8888, 2);
        framebufferMakeLinear(&fb);
        fb_initialized = true;

        plm_set_audio_enabled(plm, false);
        plm_set_loop(plm, true);
    }

    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);

    u64 last_tick = armGetSystemTick();
    u64 tick_freq = armGetSystemTickFreq();

    while (appletMainLoop())
    {
        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);
        if (kDown & HidNpadButton_Plus) break;

        if (plm) {
            u64 current_tick = armGetSystemTick();
            double delta = (double)(current_tick - last_tick) / tick_freq;
            last_tick = current_tick;

            // Step the decoder
            plm_decode(plm, delta);
        }
    }

    // Clean up
    if (plm) plm_destroy(plm);
    if (fb_initialized) framebufferClose(&fb);
    romfsExit();
    
    return 0; 
}