#ifndef _SPINNAKER_UTILITIES_H
#define _SPINNAKER_UTILITIES_H

#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <string>

#include "sleep_ms.h"

//enum trigger_type
//{
//    SOFTWARE = 0,
//    HARDWARE = 1
//};
//
//const std::vector<Spinnaker::GenICam::gcstring> trigger_name = {"Software", "Line0"};

const uint64_t width_mod = 16;
const uint64_t x_offset_mod = 4;

const uint64_t height_mod = 2;
const uint64_t y_offset_mod = 2;

// ----------------------------------------------------------------------------------------
//template<typename T>
//class node_property
//{
//public:
//    T value;
//    Spinnaker::GenICam::gcstring property_name;
//
//    node_property() {}
//
//};

//typedef struct node_property node_property;

// ----------------------------------------------------------------------------------------
inline std::ostream& operator<< (std::ostream& out, const Spinnaker::LibraryVersion& item)
{
    // Print out current library version
    out << "Spinnaker library version: " << item.major << "." << item.minor << "." << item.type << "." << item.build << std::endl;
    return out;
}

// ----------------------------------------------------------------------------------------
inline std::ostream& operator<< (std::ostream& out, const Spinnaker::GenApi::FeatureList_t& item)
{
    out << "Camera Information: " << std::endl;
    out << "  Serial Number:     " << (*(item.begin() + 1))->ToString() << std::endl;
    out << "  Camera Model:      " << (*(item.begin() + 3))->ToString() << std::endl;
    out << "  Camera Vendor:     " << (*(item.begin() + 2))->ToString() << std::endl;
    out << "  Firmware version:  " << (*(item.begin() + 7))->ToString() << std::endl;

    return out;
}

// ----------------------------------------------------------------------------------------
inline std::ostream& operator<< (std::ostream& out, const Spinnaker::CameraPtr& item)
{
    Spinnaker::GenApi::CStringPtr sn = item->GetTLDeviceNodeMap().GetNode("DeviceSerialNumber");
    Spinnaker::GenApi::CStringPtr model = item->GetTLDeviceNodeMap().GetNode("DeviceModelName");
    Spinnaker::GenApi::CStringPtr vn = item->GetTLDeviceNodeMap().GetNode("DeviceVendorName");
    Spinnaker::GenApi::CStringPtr fw = item->GetTLDeviceNodeMap().GetNode("DeviceVersion");

    out << "Camera Information: " << std::endl;
    out << "  Serial Number:     " << sn->ToString() << std::endl;
    out << "  Camera Model:      " << model->ToString() << std::endl;
    out << "  Camera Vendor:     " << vn->ToString() << std::endl;
    out << "  Firmware version:  " << fw->ToString() << std::endl;

    return out;
}


// ----------------------------------------------------------------------------------------
void print_device_info(std::ostream& out, Spinnaker::GenApi::INodeMap& node_map)
{
    Spinnaker::GenApi::FeatureList_t features;
    Spinnaker::GenApi::CCategoryPtr category = node_map.GetNode("DeviceInformation");
    if (Spinnaker::GenApi::IsAvailable(category) && Spinnaker::GenApi::IsReadable(category))
    {
        category->GetFeatures(features);
        out << features;
    }
    else
    {
        out << "Device control information not available." << std::endl;
    }
    out << std::endl;

}

// ----------------------------------------------------------------------------------------
uint32_t get_camera_selection(Spinnaker::CameraList& cam_list, uint32_t& cam_index, std::vector<std::string> &cam_sn)
{
    uint32_t idx;
    std::string console_input;
    cam_sn.clear();

    uint32_t num_cams = cam_list.GetSize();
    std::cout << "Number of cameras detected: " << num_cams << std::endl;

    for (idx = 0; idx < num_cams; ++idx)
    {
        Spinnaker::CameraPtr cam = cam_list.GetByIndex(idx);
        Spinnaker::GenApi::CStringPtr sn = cam->GetTLDeviceNodeMap().GetNode("DeviceSerialNumber");
        Spinnaker::GenApi::CStringPtr model = cam->GetTLDeviceNodeMap().GetNode("DeviceModelName");
        cam_sn.push_back(std::string(sn->ToString()));
        std::cout << "[" << idx << "] " << model->ToString() << " - Serial Number: " << cam_sn[idx] << std::endl;
    }

    if (num_cams > 0)
    {
        std::cout << "Select Camera Index: ";
        std::getline(std::cin, console_input);
        try {
            cam_index = std::stoi(console_input);
        }
        catch (std::exception e)
        {
            std::cout << "Error reading in camera index: " << e.what() << std::endl;
            exit(0);
        }
        std::cout << std::endl;
    }

    return num_cams;
}

// ----------------------------------------------------------------------------------------
void query_interfaces(Spinnaker::InterfacePtr pi)
{
    try {

        //Spinnaker::GenApi::INodeMap& node_map_interface = pi->GetTLNodeMap();

        pi->UpdateCameras();

        Spinnaker::CameraList camera_list = pi->GetCameras();

        // Retrieve number of cameras
        uint32_t num_cams = camera_list.GetSize();
        // Return if no cameras detected
        if (num_cams == 0)
        {
            std::cout << "No devices detected." << std::endl;
            return;
        }

        // Print device vendor and model name for each camera on the interface
        for (uint32_t idx = 0; idx < num_cams; ++idx)
        {
            //
            // Select camera
            //
            // *** NOTES ***
            // Each camera is retrieved from a camera list with an index. If
            // the index is out of range, an exception is thrown.
            //
            Spinnaker::CameraPtr p_cam = camera_list.GetByIndex(idx);

            // Retrieve TL device nodemap; please see NodeMapInfo example for
            // additional comments on transport layer nodemaps
            Spinnaker::GenApi::INodeMap& node_map_TL_device = p_cam->GetTLDeviceNodeMap();

            std::cout << "Device " << idx << " ";

            // Print device vendor name and device model name
            //
            // *** NOTES ***
            // Grabbing node information requires first retrieving the node and
            // then retrieving its information. There are two things to keep in
            // mind. First, a node is distinguished by type, which is related
            // to its value's data type. Second, nodes should be checked for
            // availability and readability/writability prior to making an
            // attempt to read from or write to the node.
            //
            Spinnaker::GenApi::CStringPtr vendor_name = node_map_TL_device.GetNode("DeviceVendorName");
            if (IsAvailable(vendor_name) && IsReadable(vendor_name))
            {
                //gcstring deviceVendorName = vendor_name->ToString();

                std::cout << vendor_name->ToString() << " ";
            }

            Spinnaker::GenApi::CStringPtr model_name = node_map_TL_device.GetNode("DeviceModelName");
            if (IsAvailable(model_name) && IsReadable(model_name))
            {
                //gcstring deviceModelName = model_name->ToString();

                std::cout << model_name->ToString() << std::endl;
            }
        }

        camera_list.Clear();

    }
    catch (Spinnaker::Exception & e)
    {
        std::cout << e.what() << std::endl;
    }
}


// ----------------------------------------------------------------------------------------
void set_image_size(Spinnaker::CameraPtr& cam, uint64_t &height, uint64_t &width, uint64_t &y_offset, uint64_t &x_offset)
{
    uint64_t rem = 0;

    // check each of the input values to make sure that they are correct for the parameter
    rem = height % height_mod;
    if (rem != 0)
        height = height - rem;

    rem = width % width_mod;
    if (rem != 0)
        width = width - rem;

    rem = y_offset % y_offset_mod;
    if (rem != 0)
        y_offset = y_offset - rem;

    rem = x_offset % x_offset_mod;
    if (rem != 0)
        x_offset = x_offset - rem;

    width = (width > cam->Width.GetMax()) ? cam->Width.GetMax() : width;
    height = (height > cam->Height.GetMax()) ? cam->Height.GetMax() : height;

    uint64_t x_max = cam->Width.GetMax() - width;
    uint64_t y_max = cam->Height.GetMax() - height;

    x_offset = (x_offset > x_max) ? x_max : x_offset;
    y_offset = (y_offset > y_max) ? y_max : y_offset;


    // adjust all of the parameters
    cam->Width.SetValue(width);
    cam->Height.SetValue(height);
    cam->OffsetX.SetValue(x_offset);
    cam->OffsetY.SetValue(y_offset);

}   // end of set_image_size


// ----------------------------------------------------------------------------------------
void get_image_size(Spinnaker::CameraPtr& cam, uint64_t &height, uint64_t &width, uint64_t &y_offset, uint64_t &x_offset)
{

    width = cam->Width.GetValue();
    height = cam->Height.GetValue();
    x_offset = cam->OffsetX.GetValue();
    y_offset = cam->OffsetY.GetValue();

}   // end of get_image_size


// ----------------------------------------------------------------------------------------
void set_pixel_format(Spinnaker::CameraPtr& cam, Spinnaker::PixelFormatEnums &mode)
{
    cam->PixelFormat.SetValue(mode);

}   // end of set_pixel_format

// ----------------------------------------------------------------------------------------
void get_pixel_format(Spinnaker::CameraPtr& cam, Spinnaker::PixelFormatEnums& mode)
{
    mode = cam->PixelFormat.GetValue();

}   // end of get_pixel_format

// ----------------------------------------------------------------------------------------
void set_gain(Spinnaker::CameraPtr& cam, double &value, Spinnaker::GainAutoEnums &mode)
{
    switch (mode)
    {
    case Spinnaker::GainAutoEnums::GainAuto_Continuous:
        cam->GainAuto.SetValue(mode);
        break;

    case Spinnaker::GainAutoEnums::GainAuto_Off:
    case Spinnaker::GainAutoEnums::GainAuto_Once:
        cam->GainAuto.SetValue(mode);
        sleep_ms(500);

        value = (value > cam->Gain.GetMax()) ? cam->Gain.GetMax() : value;
        value = (value < cam->Gain.GetMin()) ? cam->Gain.GetMin() : value;

        cam->Gain.SetValue(value);
        break;           
    }

}   // end of set_gain


// ----------------------------------------------------------------------------------------
void get_gain(Spinnaker::CameraPtr& cam, double& value, Spinnaker::GainAutoEnums& mode)
{
    mode = cam->GainAuto.GetValue();
    value = cam->Gain.GetValue();

}   // end of get_gain


// ----------------------------------------------------------------------------------------
void set_exposure(Spinnaker::CameraPtr& cam, double& value, Spinnaker::ExposureAutoEnums& mode)
{
    switch (mode)
    {
    case Spinnaker::ExposureAutoEnums::ExposureAuto_Continuous:
        cam->ExposureAuto.SetValue(mode);
        break;

    case Spinnaker::ExposureAutoEnums::ExposureAuto_Off:
    case Spinnaker::ExposureAutoEnums::ExposureAuto_Once:
        cam->ExposureAuto.SetValue(mode);
        sleep_ms(500);
        value = (value > cam->ExposureTime.GetMax()) ? cam->ExposureTime.GetMax() : value;
        cam->ExposureTime.SetValue(value);
        break;
    }

}   // end of set_exposure


// ----------------------------------------------------------------------------------------
void get_exposure(Spinnaker::CameraPtr& cam, double& value, Spinnaker::ExposureAutoEnums& mode)
{
    mode = cam->ExposureAuto.GetValue();
    value = cam->ExposureTime.GetValue();

}   // end of get_exposure

// ----------------------------------------------------------------------------------------
void set_acquisition(Spinnaker::CameraPtr& cam, double& value, Spinnaker::AcquisitionModeEnums& mode)
{

    switch (mode)
    {
    case Spinnaker::AcquisitionModeEnums::AcquisitionMode_Continuous:
        cam->AcquisitionMode.SetValue(mode);
        value = (value > cam->AcquisitionFrameRate.GetMax()) ? cam->AcquisitionFrameRate.GetMax() : value;
        value = (value > cam->AcquisitionFrameRate.GetMin()) ? cam->AcquisitionFrameRate.GetMin() : value;
        cam->AcquisitionFrameRate.SetValue(value);
        break;

    case Spinnaker::AcquisitionModeEnums::AcquisitionMode_SingleFrame:
        cam->AcquisitionMode.SetValue(mode);
        value = (value > cam->AcquisitionFrameRate.GetMax()) ? cam->AcquisitionFrameRate.GetMax() : value;
        value = (value > cam->AcquisitionFrameRate.GetMin()) ? cam->AcquisitionFrameRate.GetMin() : value;
        cam->AcquisitionFrameRate.SetValue(value);
        break;

    case Spinnaker::AcquisitionModeEnums::AcquisitionMode_MultiFrame:
        cam->AcquisitionMode.SetValue(mode);
        value = (value > cam->AcquisitionFrameCount.GetMax()) ? cam->AcquisitionFrameCount.GetMax() : value;
        value = (value > cam->AcquisitionFrameCount.GetMin()) ? cam->AcquisitionFrameCount.GetMin() : value;
        cam->AcquisitionFrameCount.SetValue((int64_t)value);
        break;
    }

}   // end of set_acquistion

// ----------------------------------------------------------------------------------------
void get_acquisition(Spinnaker::CameraPtr& cam, double& value, Spinnaker::AcquisitionModeEnums& mode)
{
    mode = cam->AcquisitionMode.GetValue();
    switch (mode)
    {
        case Spinnaker::AcquisitionModeEnums::AcquisitionMode_Continuous:
        case Spinnaker::AcquisitionModeEnums::AcquisitionMode_SingleFrame:
            value = cam->AcquisitionFrameRate.GetValue();
            break;

        case Spinnaker::AcquisitionModeEnums::AcquisitionMode_MultiFrame:
            value = (double)cam->AcquisitionFrameCount.GetValue();
            break;
    }

}   // end of get_acquisition

/*
// ----------------------------------------------------------------------------------------
int configure_exposure(Spinnaker::GenApi::INodeMap& node_map, double value)
{
    int result = 0;

    // Turn off automatic exposure mode
    //
    // *** NOTES ***
    // Automatic exposure prevents the manual configuration of exposure
    // time and needs to be turned off.
    //
    // *** LATER ***
    // Exposure time can be set automatically or manually as needed. This
    // example turns automatic exposure off to set it manually and back
    // on in order to return the camera to its default state.
    //
    Spinnaker::GenApi::CEnumerationPtr exposure_node = node_map.GetNode("ExposureAuto");
    if (!Spinnaker::GenApi::IsAvailable(exposure_node) || !Spinnaker::GenApi::IsWritable(exposure_node))
    {
        std::cout << "Unable to disable automatic exposure (node retrieval). Aborting..." << std::endl << std::endl;
        return -1;
    }

    Spinnaker::GenApi::CEnumEntryPtr exposure_mode = exposure_node->GetEntryByName("Off");
    if (!Spinnaker::GenApi::IsAvailable(exposure_mode) || !Spinnaker::GenApi::IsReadable(exposure_mode))
    {
        std::cout << "Unable to disable automatic exposure (enum entry retrieval). Aborting..." << std::endl << std::endl;
        return -1;
    }

    exposure_node->SetIntValue(exposure_mode->GetValue());


    //
    // Set exposure time manually; exposure time recorded in microseconds
    //
    // *** NOTES ***
    // The node is checked for availability and writability prior to the
    // setting of the node. Further, it is ensured that the desired exposure
    // time does not exceed the maximum. Exposure time is counted in
    // microseconds. This information can be found out either by
    // retrieving the unit with the GetUnit() method or by checking SpinView.
    //
    Spinnaker::GenApi::CFloatPtr exposure_time = node_map.GetNode("ExposureTime");
    if (!Spinnaker::GenApi::IsAvailable(exposure_time) || !Spinnaker::GenApi::IsWritable(exposure_time))
    {
        std::cout << "Unable to set exposure time. Aborting..." << std::endl << std::endl;
        return -1;
    }

    // Ensure desired exposure time does not exceed the maximum
    const double max_exposure_time = exposure_time->GetMax();

    if (value > max_exposure_time)
    {
        value = max_exposure_time;
    }

    exposure_time->SetValue(value);

    return result;

}   // end of configure_exposure
*/


// ----------------------------------------------------------------------------------------
void set_trigger(Spinnaker::CameraPtr& cam, Spinnaker::TriggerSourceEnums &source, Spinnaker::TriggerModeEnums& mode)
{
    cam->TriggerMode.SetValue(mode);
    sleep_ms(1000);
    cam->TriggerSource.SetValue(source);

}   // end of set_trigger

/*
// ----------------------------------------------------------------------------------------
int configure_trigger(Spinnaker::GenApi::INodeMap& node_map, const trigger_type source, bool on_off)
{
    int result = 0;

    // Ensure trigger mode off
    //
    // *** NOTES ***
    // The trigger must be disabled in order to configure whether the source
    // is software or hardware.
    //
    Spinnaker::GenApi::CEnumerationPtr trigger_mode = node_map.GetNode("TriggerMode");
    if (!Spinnaker::GenApi::IsAvailable(trigger_mode) || !Spinnaker::GenApi::IsReadable(trigger_mode))
    {
        std::cout << "Unable to disable trigger mode (node retrieval). Aborting..." << std::endl;
        return -1;
    }

    Spinnaker::GenApi::CEnumEntryPtr trigger_status = trigger_mode->GetEntryByName("Off");
    if (!Spinnaker::GenApi::IsAvailable(trigger_status) || !Spinnaker::GenApi::IsReadable(trigger_status))
    {
        std::cout << "Unable to disable trigger mode (enum entry retrieval). Aborting..." << std::endl;
        return -1;
    }
    trigger_mode->SetIntValue(trigger_status->GetValue());
    //std::cout << "Trigger mode disabled..." << std::endl;

    //
    // Select trigger source
    //
    // *** NOTES ***
    // The trigger source must be set to hardware or software while trigger
    // mode is off.
    //
    Spinnaker::GenApi::CEnumerationPtr trigger_source = node_map.GetNode("TriggerSource");
    if (!Spinnaker::GenApi::IsAvailable(trigger_source) || !Spinnaker::GenApi::IsWritable(trigger_source))
    {
        std::cout << "Unable to set trigger mode (node retrieval). Aborting..." << std::endl;
        return -1;
    }

    //if (source == SOFTWARE)
    //{
    //    // Set trigger mode to software
    //    Spinnaker::GenApi::CEnumEntryPtr trigger_source_soft = trigger_source->GetEntryByName("Software");
    //    if (!Spinnaker::GenApi::IsAvailable(trigger_source_soft) || !Spinnaker::GenApi::IsReadable(trigger_source_soft))
    //    {
    //        std::cout << "Unable to set trigger mode (enum entry retrieval). Aborting..." << std::endl;
    //        return -1;
    //    }
    //    trigger_source->SetIntValue(trigger_source_soft->GetValue());
    //    //std::cout << "Trigger source set to software..." << std::endl;
    //}
    //else if (source == HARDWARE)
    //{
    //    // Set trigger mode to hardware ('Line0')
    //    Spinnaker::GenApi::CEnumEntryPtr trigger_source_hard = trigger_source->GetEntryByName("Line0");
    //    if (!Spinnaker::GenApi::IsAvailable(trigger_source_hard) || !Spinnaker::GenApi::IsReadable(trigger_source_hard))
    //    {
    //        std::cout << "Unable to set trigger mode (enum entry retrieval). Aborting..." << std::endl;
    //        return -1;
    //    }
    //    trigger_source->SetIntValue(trigger_source_hard->GetValue());
    //    //std::cout << "Trigger source set to hardware..." << std::endl;
    //}

    Spinnaker::GenApi::CEnumEntryPtr ts = trigger_source->GetEntryByName(trigger_name[source]);
    if (!Spinnaker::GenApi::IsAvailable(ts) || !Spinnaker::GenApi::IsReadable(ts))
    {
        std::cout << "Unable to set trigger mode (enum entry retrieval). Aborting..." << std::endl;
        return -1;
    }
    trigger_source->SetIntValue(ts->GetValue());
    std::cout << "Trigger source set to " << trigger_name[source] << std::endl;

    //
    // Turn trigger mode on
    //
    // *** LATER ***
    // Once the appropriate trigger source has been set, turn trigger mode
    // on in order to retrieve images using the trigger.
    //
    if (on_off)
    {
        //Spinnaker::GenApi::CEnumEntryPtr ptrTriggerModeOn = ptrTriggerMode->GetEntryByName("On");
        trigger_status = trigger_mode->GetEntryByName("On");
        if (!Spinnaker::GenApi::IsAvailable(trigger_status) || !Spinnaker::GenApi::IsReadable(trigger_status))
        {
            std::cout << "Unable to enable trigger mode (enum entry retrieval). Aborting..." << std::endl;
            return -1;
        }
        trigger_mode->SetIntValue(trigger_status->GetValue());

        sleep_ms(1000);         // Blackfly and Flea3 GEV cameras need 1 second delay after trigger mode is turned on
        std::cout << "Trigger mode turned back on..." << std::endl << std::endl;
    }

    return result;

}   // end of configure_trigger

*/


// ----------------------------------------------------------------------------------------
void fire_software_trigger(Spinnaker::CameraPtr& cam)
{
    cam->TriggerSoftware.Execute();
    sleep_ms(2000);
}

/*
// ----------------------------------------------------------------------------------------
int fire_software_trigger(Spinnaker::GenApi::INodeMap& node_map)
{
    int result = 0;

    // Use trigger to capture image
    //
    // *** NOTES ***
    // The software trigger only feigns being executed by the Enter key;
    // what might not be immediately apparent is that there is not a
    // continuous stream of images being captured; in other examples that
    // acquire images, the camera captures a continuous stream of images.
    // When an image is retrieved, it is plucked from the stream.

    // Execute software trigger
    Spinnaker::GenApi::CCommandPtr trigger_command = node_map.GetNode("TriggerSoftware");
    if (!Spinnaker::GenApi::IsAvailable(trigger_command) || !Spinnaker::GenApi::IsWritable(trigger_command))
    {
        std::cout << "Unable to execute trigger. Aborting..." << std::endl;
        return -1;
    }
    trigger_command->Execute();
    sleep_ms(2000);         // Blackfly and Flea3 GEV cameras need 2 second delay after software trigger

    return result;
}	// end of fire_software_trigger
*/


// ----------------------------------------------------------------------------------------
//int acquire_image(Spinnaker::CameraPtr cam, Spinnaker::GenApi::INodeMap& node_map)//, Spinnaker::GenApi::INodeMap& node_map_TL_device)
int acquire_image(Spinnaker::CameraPtr &cam, Spinnaker::ImagePtr &image)//, Spinnaker::GenApi::INodeMap& node_map_TL_device)
{
    int result = 0;

    //// Set acquisition mode to continuous
    //Spinnaker::GenApi::CEnumerationPtr acquisition_node = node_map.GetNode("AcquisitionMode");
    //if (!Spinnaker::GenApi::IsAvailable(acquisition_node) || !Spinnaker::GenApi::IsWritable(acquisition_node))
    //{
    //    std::cout << "Unable to set acquisition mode to continuous (node retrieval). Aborting..." << std::endl << std::endl;
    //    return -1;
    //}

    //Spinnaker::GenApi::CEnumEntryPtr acquisition_mode = acquisition_node->GetEntryByName("Continuous");
    //if (!Spinnaker::GenApi::IsAvailable(acquisition_mode) || !Spinnaker::GenApi::IsReadable(acquisition_mode))
    //{
    //    std::cout << "Unable to set acquisition mode to continuous (enum entry retrieval). Aborting..." << std::endl << std::endl;
    //    return -1;
    //}

    //int64_t acquisition_value = acquisition_mode->GetValue();

    //acquisition_node->SetIntValue(acquisition_value);

    //cout << "Acquisition mode set to continuous..." << endl;

    // Begin acquiring images
    //cam->BeginAcquisition();

    // Retrieve next received image
    //
    // *** NOTES ***
    // Capturing an image houses images on the camera buffer. Trying
    // to capture an image that does not exist will hang the camera.
    //
    // *** LATER ***
    // Once an image from the buffer is saved and/or no longer
    // needed, the image must be released in order to keep the
    // buffer from filling up.
    //
    Spinnaker::ImagePtr ptr_img = cam->GetNextImage();
    //image = cam->GetNextImage();

    //
    // Ensure image completion
    //
    // *** NOTES ***
    // Images can easily be checked for completion. This should be
    // done whenever a complete image is expected or required.
    // Further, check image status for a little more insight into
    // why an image is incomplete.
    //
    if (ptr_img->IsIncomplete())
    {
        // Retrieve and print the image status description
        std::cout << "Image incomplete: " << Spinnaker::Image::GetImageStatusDescription(ptr_img->GetImageStatus())
            << "..." << std::endl << std::endl;
    }

    image = ptr_img->Convert(Spinnaker::PixelFormat_BGR8);

    //
    // Release image
    //
    // *** NOTES ***
    // Images retrieved directly from the camera (i.e. non-converted
    // images) need to be released in order to keep from filling the
    // buffer.
    //
    ptr_img->Release();


    // End acquisition
    //cam->EndAcquisition();

    return result;
}


#endif  // _SPINNAKER_UTILITIES_H
