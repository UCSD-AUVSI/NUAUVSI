#include <iostream>
#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"
#include "SkynetSeg_BackboneInterface.hpp"
#include "Segmentation_SSEG_and_CSEG_and_Merger.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/SharedUtils_OpenCV.hpp" //for saveImage


void SkynetSeg :: execute(imgdata_t *imdata, std::string args)
{
	std::cout << "SkynetSeg, ID: " << imdata->id  << ", CropID: " << imdata->cropid << std::endl;

	if(imdata->image_data->empty() == false)
	{
        Segmentation_SSEG_and_CSEG_and_Merger SkynetSegmentation_module_instance;

		//valgrind doesn't like this ("possibly lost")
		cv::Mat cropped_input_image = cv::imdecode(*(imdata->image_data), CV_LOAD_IMAGE_COLOR);

		std::vector<cv::Mat> returned_SSEGs;
		std::vector<cv::Scalar> returned_SSEG_colors;
		std::vector<cv::Mat> returned_CSEGs;
		std::vector<cv::Scalar> returned_CSEG_colors;

		//these two are optional; they tell the module where to save photos, and what name to assign this crop
		std::string folder_to_save_SSEGs_and_CSEGs("../../output_images");
        std::string name_of_input_crop = std::to_string(imdata->id) + "_" + std::to_string(imdata->cropid);
		bool save_ssegs_and_csegs = check_if_directory_exists(folder_to_save_SSEGs_and_CSEGs);
		
#if 0
		if(save_ssegs_and_csegs) {
			saveImage(cropped_input_image,
				folder_to_save_SSEGs_and_CSEGs + std::string("/") + name_of_input_crop + std::string("__crop.jpg"));
		}
#endif

		SkynetSegmentation_module_instance.DoModule(cropped_input_image,
										                    &returned_SSEGs,
										                    &returned_SSEG_colors,
										                    &returned_CSEGs,
										                    &returned_CSEG_colors,
															//optional: save SSEGs and CSEGs
															&folder_to_save_SSEGs_and_CSEGs,
															save_ssegs_and_csegs,
															&name_of_input_crop);


		//pack found images (if any) into the message struct
		for(std::vector<cv::Mat>::iterator iter_sseg = returned_SSEGs.begin();
				iter_sseg != returned_SSEGs.end(); iter_sseg++)
		{
			/*std::vector<unsigned char> *newarr = new std::vector<unsigned char>();
			cv::imencode(".jpg", *iter_sseg, *newarr);
			imdata->sseg_image_data->push_back(newarr);*/
			
			std::vector<unsigned char> *newarr = new std::vector<unsigned char>();
			std::vector<int> param = std::vector<int>(2);
			param[0] = CV_IMWRITE_PNG_COMPRESSION;
			param[1] = 3; //default(3)  0-9, where 9 is smallest compressed size.
			cv::imencode(".png", *iter_sseg, *newarr, param);
			imdata->sseg_image_data->push_back(newarr);
		}

		for(std::vector<cv::Mat>::iterator iter_cseg = returned_CSEGs.begin();
				iter_cseg != returned_CSEGs.end(); iter_cseg++)
		{
			/*std::vector<unsigned char> *newarr = new std::vector<unsigned char>();
			cv::imencode(".jpg", *iter_cseg, *newarr);
			imdata->cseg_image_data->push_back(newarr);*/
			
			std::vector<unsigned char> *newarr = new std::vector<unsigned char>();
			std::vector<int> param = std::vector<int>(2);
			param[0] = CV_IMWRITE_PNG_COMPRESSION;
			param[1] = 3; //default(3)  0-9, where 9 is smallest compressed size.
			cv::imencode(".png", *iter_cseg, *newarr, param);
			imdata->cseg_image_data->push_back(newarr);
		}


        if(returned_SSEGs.empty() == false)
        {
            for(std::vector<cv::Scalar>::iterator iter_sseg_color = returned_SSEG_colors.begin();
                    iter_sseg_color != returned_SSEG_colors.end(); iter_sseg_color++)
            {
				imdata->scolorR = (*iter_sseg_color)[2];
				imdata->scolorG = (*iter_sseg_color)[1];
				imdata->scolorB = (*iter_sseg_color)[0];
            }
        }
        if(returned_CSEGs.empty() == false)
        {
            for(std::vector<cv::Scalar>::iterator iter_cseg_color = returned_CSEG_colors.begin();
                    iter_cseg_color != returned_CSEG_colors.end(); iter_cseg_color++)
            {
				imdata->ccolorR = (*iter_cseg_color)[2];
				imdata->ccolorG = (*iter_cseg_color)[1];
				imdata->ccolorB = (*iter_cseg_color)[0];
            }
        }
	}
	else
		std::cout << "SkynetSeg module wasn't given a crop from saliency!" << std::endl;

	setDone(imdata, SEG);
}
