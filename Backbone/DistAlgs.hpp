#ifndef DIST_ALGS_H
#define DIST_ALGS_H

#include "ImagePush/stub_push.hpp"
#include "ImagePush/FilePush/file_push.hpp"
#include "ImagePush/FolderPush/folder_push.hpp"

#include "Orthorect/stub_orthorect.hpp"

#include "Saliency/stub_saliency.hpp"
#include "Saliency/SSaliency/ssaliency.hpp"
#include "GUISaliency/GUISaliency.hpp"

#include "Recognition/Segmentation/stub_seg.hpp"
#include "Recognition/Segmentation/SkynetSegmentation/SkynetSeg_BackboneInterface.hpp"
#include "GUIRec/GUIRec.hpp"

#include "Recognition/ShapeRec/stub_srec.hpp"
#include "Recognition/ShapeRec/PolygonShapeRec/Shaperec_BackboneInterface.hpp"

#include "Recognition/OCR/stub_ocr.hpp"
#include "Recognition/OCR/TesseractOCR/TessOCR_BackboneInterface.hpp"
#include "Recognition/OCR/GOCR/GOCR_BackboneInterface.hpp"

#include "Verification/stub_verif.hpp"
#include "Verification/DisplayVerif/display_verif.hpp"

#endif