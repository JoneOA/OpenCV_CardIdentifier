// CardIdentifier.cpp : Defines the entry point for the application.
//

#include "CardIdentifier.h"
#include <iostream>
#include <filesystem>
#include <opencv2/opencv.hpp>

using namespace std;

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	if (event == cv::EVENT_LBUTTONDOWN)
	{
		cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (event == cv::EVENT_RBUTTONDOWN)
	{
		cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (event == cv::EVENT_MBUTTONDOWN)
	{
		cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
}

int main()
{
	std::vector<cv::Point2f> cardCorners, warpedCorners(4), midPoints(4);

	std::vector<std::vector<cv::Point>> edges, numberCont, warpedCont;
	std::vector<std::vector<cv::Point>> rectangles;
	std::vector<cv::Point> shapes;

	cv::VideoCapture cap(0);

	int threshL = 245;

	cv::namedWindow("Sliders", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("w", cv::WINDOW_AUTOSIZE);
	cv::createTrackbar("ThreshL", "Sliders", &threshL, 255);

	cv::Mat cimg, img, cardImg, M, warpedImg, warpedImg2, result, sub;

	set<filesystem::path> sortedCards;
	cv::setMouseCallback("w", CallBackFunc, NULL);
	while (true) {
		std::string cardNumber;
		int nonZeroCount = 10000000;
		cap >> cardImg;

		cardImg.copyTo(cimg);
		cv::cvtColor(cardImg, cardImg, cv::COLOR_BGR2GRAY);
		cv::threshold(cardImg, cardImg, threshL, 255, cv::THRESH_BINARY_INV);

		cv::findContours(cardImg, edges, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

		for (std::vector<cv::Point> edge : edges) {
			cv::approxPolyDP(edge, shapes, 0.01*cv::arcLength(edge, true), true);
			if (shapes.size() == 4) { 
				rectangles.push_back(shapes);
			}
		}
		
		for (int i = 0; i < rectangles.size(); i++) {
			if (cv::contourArea(rectangles[i]) > 200) {
				cv::drawContours(cimg, rectangles, i, cv::Scalar(255, 0, 255));

				for (int j = 0; j < rectangles[i].size(); j++) {
					cardCorners.push_back(cv::Point2f((float)rectangles[i][j].x, (float)rectangles[i][j].y));
				}

				midPoints[0] = (cardCorners[0] + cardCorners[1]) / 2;
				midPoints[1] = (cardCorners[1] + cardCorners[2]) / 2;
				midPoints[2] = (cardCorners[2] + cardCorners[3]) / 2;
				midPoints[3] = (cardCorners[3] + cardCorners[0]) / 2;

				warpedCorners[0].x = 0;
				warpedCorners[0].y = 0;
				warpedCorners[1].x = (float)norm(midPoints[1] - midPoints[3]);
				warpedCorners[1].y = 0;
				warpedCorners[2].x = warpedCorners[1].x;
				warpedCorners[2].y = (float)norm(midPoints[0] - midPoints[2]);
				warpedCorners[3].x = 0;
				warpedCorners[3].y = warpedCorners[2].y;

  				cv::Size warpedImgSize(cvRound(warpedCorners[2].x), cvRound(warpedCorners[2].y));

				M = cv::getPerspectiveTransform(cardCorners, warpedCorners);

				cv::warpPerspective(cardImg, warpedImg, M, warpedImgSize); 

				if (warpedImgSize.width > warpedImgSize.height) {
					cv::rotate(warpedImg, warpedImg, cv::ROTATE_90_CLOCKWISE);
				}
				

				cv::Size s(20, 70);
				cv::Point p(10, 35);
				cv::getRectSubPix(warpedImg, s, p, warpedImg);

				//cv::threshold(warpedImg, warpedImg, 123, 255, cv::THRESH_BINARY);

				cv::resize(warpedImg, warpedImg, cv::Size(70, 120));
				cv::findContours(warpedImg, warpedCont, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

				for (int l = 0; l < warpedCont.size(); l++) {
					if (boundingRect(warpedCont[l]).area() > 1000) {
						cv::rectangle(warpedImg, boundingRect(warpedCont[l]), cv::Scalar::all(255));
						std::cout << l << ": " << boundingRect(warpedCont[l]).area() << "; ";
					}
				}
				std::cout << "\n";

				for (auto& file : filesystem::directory_iterator("C:\\Users\\Owain\\source\\repos\\CardIdentifier\\CardIdentifier\\Resources\\Numbers")) {
					img = cv::imread(file.path().string());
					cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
					cv::GaussianBlur(img, img, cv::Size(3, 3), 0);
					cv::threshold(img, img, 123, 255, cv::THRESH_BINARY_INV);

					cv::findContours(img, numberCont, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
					cv::Size S;
					cv::Point P;
					for (int k = 0; k < numberCont.size(); k++) {
						cv::Rect numR = boundingRect(numberCont[k]);
						if (numR.area() > 20000) {
							cv::rectangle(img, numR, cv::Scalar::all(255));
							S = numR.size();
							P = cv::Point(numR.x + numR.width / 2, numR.y + numR.height / 2);
						}
					}
					//cv::getRectSubPix(img, S, P, img);
					//cv::resize(img, img, cv::Size(70, 120));

					//cv::subtract(warpedImg, img, sub);

					/*if (cv::countNonZero(sub) < nonZeroCount) {
						cardNumber = file.path().string();
						nonZeroCount = cv::countNonZero(sub);
						std::cout << cardNumber << ": " << nonZeroCount << "\n";
					}*/

					cv::imshow("mg", img);
					cv::imshow("w", warpedImg);
					//cv::imshow("sub", sub);
					cv::waitKey(0);
				}

				cardCorners.clear();
			}
		}
		cv::imshow("E", cimg);
		cv::imshow("D", cardImg);
		rectangles.clear();

		std::cout << cardNumber << "\n";
		cv::waitKey(0);
	}

	return 0;
}