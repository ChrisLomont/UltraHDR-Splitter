# UltraHDR Splitter

Split UltraHDR jpeg images into the base image, the gain map image, and the gain map data

Example images are of two scenes, and most are available in RAW, original Jpeg from device, and raw processed through Lightroom. Android Phone HDR have a single channel gain map, and Lightroom processed have three channel gain map. Each table entry is for scene 1/scene 2 file present.

| Camera      | Original Ultra HDR | Lightroom processed | RAW  |
| ----------- | ------------------ | ------------------- | ---- |
| Pixel 8 Pro | Y/Y                | Y/Y                 | Y/Y  |
| Galaxy S24  | Y/Y                | N/Y                 | N/Y  |
| Galaxy S22  | Y/Y                | N/Y                 | N/Y  |
| Pixel 6     | Y/Y                | Y/Y                 | Y/Y  |

For example, here is the original UltraHDR image from the phone, 

![Pixel8Pro_1_Original](.\HDR\Pixel8Pro_1_Original.jpg)



Here are the SDR and (single channel) gain map images from the phone after being split by this tool,  ![Pixel8Pro_1_Original_split_1](.\Output\Pixel8Pro_1_Original_split_1.jpg)



![Pixel8Pro_1_Original_split_2](.\Output\Pixel8Pro_1_Original_split_2.jpg)



and here are the Lightroom processed SDR and (three channel) gain map, split by this tool:

![Pixel8Pro_1_LR_split_1](.\Output\Pixel8Pro_1_LR_split_1.jpg)





![Pixel8Pro_1_LR_split_2](.\Output\Pixel8Pro_1_LR_split_2.jpg)
