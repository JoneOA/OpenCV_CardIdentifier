// CardIdentifier.cpp : Defines the entry point for the application.
//

#include "CardIdentifier.h"
#include <iostream>
#include <filesystem>
#include <opencv2/opencv.hpp>

using namespace std;

int main()
{
	std::vector<cv::Point2f> cardCorners, warpedCorners(4), midPoints(4);

	std::vector<std::vector<cv::Point>> edges, numberCont, warpedCont;
	std::vector<std::vector<cv::Point>> rectangles;
	std::vector<cv::Point> shapes;

	cv::VideoCapture cap(0);

	int threshL = 245;

	cv::namedWindow("Sliders", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("Warped", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("Result", cv::WINDOW_AUTOSIZE);
	cv::createTrackbar("ThreshL", "Sliders", &threshL, 255);

	cv::Mat img, cardImg, M, warpedImg, warpedImg2, result;

	set<filesystem::path> sortedCards;


	//for (auto& file : filesystem::directory_iterator("C:\\Users\\Owain\\source\\repos\\CardIdentifier\\CardIdentifier\\Resources\\Suit")) {
	//	img = cv::imread(file.path().string());

	//	cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);

	//	cv::GaussianBlur(img, img, cv::Size(3, 3), 0);

	//	cv::threshold(img, img, 123, 255, cv::THRESH_BINARY_INV);

	//	cv::imshow("Suit", img);

	//	cv::waitKey(0);

	//}

	while (true) {
		cap >> cardImg;

		cardImg.copyTo(img);
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
				cv::drawContours(img, rectangles, i, cv::Scalar(255, 0, 255));

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

				cv::resize(warpedImg, warpedImg, cv::Size(620, 870));
				cv::GaussianBlur(warpedImg, warpedImg, cv::Size(13, 13), 0);
				cv::findContours(warpedImg, warpedCont, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
				for (int l = 0; l < warpedCont.size(); l++) {
					cv::rectangle(warpedImg, boundingRect(warpedCont[l]), cv::Scalar::all(255));
				}
				warpedImg2 = warpedImg;
				//cv::cvtColor(warpedImg, warpedImg, cv::COLOR_BGR2GRAY);
				//cv::threshold(warpedImg, warpedImg, 245, 255, cv::THRESH_BINARY_INV);

				for (auto& file : filesystem::directory_iterator("C:\\Users\\Owain\\source\\repos\\CardIdentifier\\CardIdentifier\\Resources\\Numbers")) {
					img = cv::imread(file.path().string());
					cv::resize(img, img, cv::Size(70, 120));
					cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
					cv::GaussianBlur(img, img, cv::Size(3, 3), 0);
					cv::threshold(img, img, 123, 255, cv::THRESH_BINARY_INV);

					cv::findContours(img, numberCont, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
					for (int k = 0; k < numberCont.size(); k++) {
						cv::rectangle(img, boundingRect(numberCont[k]), cv::Scalar::all(255));
					}
					cv::imshow("mg", img);
					cv::imshow("w", warpedImg);
					cv::waitKey(0);
				}

				cardCorners.clear();
			}
		}

		cv::imshow("D", cardImg);
		rectangles.clear();
		cv::waitKey(0);
	}

	return 0;
}