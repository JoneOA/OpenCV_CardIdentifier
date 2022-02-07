# OpenCV_CardIdentifier 

## Outcome
Work In Progress
## Objectives
1) Find playing card in image
2) Isolate playing card
3) Identify the value and suit of the card
## Method
Input image is thresholded: -

![Threshhold playing card](https://user-images.githubusercontent.com/54110810/152744831-93a04cd3-b814-4f99-8b27-6018c701c16f.png)

Rectangles are found from input image: -
cv::findContours -> cv::approxPolyDP -> if has 4 edges = rectangle = card

![Plaing card rectangle](https://user-images.githubusercontent.com/54110810/152744754-fab6cba4-0d81-44d7-b4c4-79c98e76790a.png)

Warp the rectange bounding the card to a rectangle: -
cv::warpPerspective

![Card isolated](https://user-images.githubusercontent.com/54110810/152744911-baebfa09-958f-430c-a123-57042328c2b3.png)

Used to compare to playing card number for identification: -
convolute images and perform subtraction, least white pixels is match (Currently working on)

![Number Template](https://user-images.githubusercontent.com/54110810/152744935-c52839a8-8043-44f5-aa14-8f19577d3a64.png)


## Improvements
