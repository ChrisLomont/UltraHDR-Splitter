# UltraHDR Splitter

Split UltraHDR jpeg images into the base image, the gain map image, and the gain map data

Example images are of two scenes, and most are available in RAW, original Jpeg from device, and raw processed through Lightroom. Android Phone HDR have a single channel gain map, and Lightroom processed have three channel gain map. Each table entry is for scene 1/scene 2 file present.

| Camera      | Original Ultra HDR | Lightroom processed | RAW  |
| ----------- | ------------------ | ------------------- | ---- |
| Pixel 8 Pro | Y/Y                | Y/Y                 | Y/Y  |
| Galaxy S24  | Y/Y                | N/Y                 | N/Y  |
| Galaxy S22  | Y/Y                | N/Y                 | N/Y  |
| Pixel 6     | Y/Y                | Y/Y                 | Y/Y  |

For example, here is the original UltraHDR image from the Pixel 8 Pro, 

![Pixel8Pro_1_Original](HDR/Pixel8Pro_1_Original.jpg)



Here are the SDR and (single channel) gain map images from the phone after being split by this tool,  ![Pixel8Pro_1_Original_split_1](Output/Pixel8Pro_1_Original_split_1.jpg)



![Pixel8Pro_1_Original_split_2](Output/Pixel8Pro_1_Original_split_2.jpg)



and here are the Lightroom processed SDR and (three channel) gain map from the RAW version from the phone, split by this tool:

![Pixel8Pro_1_LR_split_1](Output/Pixel8Pro_1_LR_split_1.jpg)





![Pixel8Pro_1_LR_split_2](Output/Pixel8Pro_1_LR_split_2.jpg)



Here's the above picture and a second example to show the comparisons

|                                       |                                                       |                                                              |                                                              |
| ------------------------------------- | ----------------------------------------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| Pixel 8 Pro original                  | ![Pixel8Pro_1_Original](HDR/Pixel8Pro_1_Original.jpg) | ![Pixel8Pro_1_Original_split_1](Output/Pixel8Pro_1_Original_split_1.jpg) | ![Pixel8Pro_1_Original_split_2](Output/Pixel8Pro_1_Original_split_2.jpg) |
| Pixel 8 Pro via RAW through Lightroom |                                                       | ![Pixel8Pro_1_LR_split_1](Output/Pixel8Pro_1_LR_split_1.jpg) | ![Pixel8Pro_1_LR_split_2](Output/Pixel8Pro_1_LR_split_2.jpg) |
| Galaxy S24 Original                   | ![GalaxyS24_2_Original](HDR/GalaxyS24_2_Original.jpg) | ![GalaxyS24_2_Original_split_1](Output/GalaxyS24_2_Original_split_1.jpg) | ![GalaxyS24_2_Original_split_2](Output/GalaxyS24_2_Original_split_2.jpg) |
| Galaxy S24 via RAW through Lightroom  |                                                       | ![GalaxyS24_2_LR_split_1](Output/GalaxyS24_2_LR_split_1.jpg) | ![GalaxyS24_2_LR_split_2](Output/GalaxyS24_2_LR_split_2.jpg) |

s
