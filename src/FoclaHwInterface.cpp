//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2011
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################
/***************************************************************//**
 * @file   FoclaHwInterface.cpp
 * @brief  This file contains Focla Hardware Interface implementation
 *
 * @author A.Kirov
 * @date   15/07/2009
 *******************************************************************/
#include "FoclaHwInterface.h"

using namespace std;
using namespace lima;
using namespace Espia::Focla;


/***************************************************************//**
 * @brief Espia::Focla::DetInfoCtrlObj class constructor
 *
 * @param[in] focla        reference to Espia::Focla::Dev object
 *******************************************************************/
DetInfoCtrlObj::DetInfoCtrlObj(Dev &focla)
	: m_focla(focla)
{
}


DetInfoCtrlObj::~DetInfoCtrlObj()
{
}


const FrameDim lima::Espia::Focla::MaxFrameDim(2048, 2048, Bpp16); // arbitrary?


void DetInfoCtrlObj::getMaxImageSize(Size &max_image_size)
{
	max_image_size = MaxFrameDim.getSize();
}


void DetInfoCtrlObj::getDetectorImageSize(Size &det_image_size)
{
	det_image_size = MaxFrameDim.getSize();
}


void DetInfoCtrlObj::getDefImageType(ImageType &def_image_type)
{
	def_image_type = MaxFrameDim.getImageType();
}


void DetInfoCtrlObj::getCurrImageType(ImageType &curr_image_type)
{
	curr_image_type = MaxFrameDim.getImageType();
}


void DetInfoCtrlObj::setCurrImageType(ImageType curr_image_type)
{
	throw LIMA_HW_EXC(NotSupported,"This function is not supported by Focla");
}


void DetInfoCtrlObj::getPixelSize(double& x_size,double& y_size)
{
	throw LIMA_HW_EXC(NotSupported,"This function is not supported by Focla");
}


void DetInfoCtrlObj::getDetectorType(std::string &det_type)
{
	det_type = "Focla";
}


void DetInfoCtrlObj::getDetectorModel(std::string &det_model)
{
	det_model = "";
}


void DetInfoCtrlObj::registerMaxImageSizeCallback(HwMaxImageSizeCallback& cb)
{
	m_mis_cb_gen.registerMaxImageSizeCallback(cb);
}

void DetInfoCtrlObj::unregisterMaxImageSizeCallback(HwMaxImageSizeCallback& cb)
{
	m_mis_cb_gen.unregisterMaxImageSizeCallback(cb);
}


void DetInfoCtrlObj::MaxImageSizeCallbackGen::setMaxImageSizeCallbackActive(
							    bool cb_active)
{
}


/***************************************************************//**
 * @brief Espia::Focla::BufferCtrlObj class constructor
 *
 * @param[in] buffer_mgr   reference to BufferCtrlMgr object
 *******************************************************************/
BufferCtrlObj::BufferCtrlObj( BufferCtrlMgr &buffer_mgr )
	: m_buffer_mgr(buffer_mgr)
{
}


BufferCtrlObj::~BufferCtrlObj()
{
}


void BufferCtrlObj::setFrameDim(const FrameDim &frame_dim)
{
	m_buffer_mgr.setFrameDim(frame_dim);
}


void BufferCtrlObj::getFrameDim(FrameDim &frame_dim)
{
	m_buffer_mgr.getFrameDim(frame_dim);
}


void BufferCtrlObj::setNbBuffers(int nb_buffers)
{
	m_buffer_mgr.setNbBuffers(nb_buffers);
}


void BufferCtrlObj::getNbBuffers(int &nb_buffers)
{
	m_buffer_mgr.getNbBuffers(nb_buffers);
}


void BufferCtrlObj::setNbConcatFrames(int nb_concat_frames)
{
	m_buffer_mgr.setNbConcatFrames(nb_concat_frames);
}


void BufferCtrlObj::getNbConcatFrames(int &nb_concat_frames)
{
	m_buffer_mgr.getNbConcatFrames(nb_concat_frames);
}


void BufferCtrlObj::getMaxNbBuffers(int &max_nb_buffers)
{
	m_buffer_mgr.getMaxNbBuffers(max_nb_buffers);

}


void *BufferCtrlObj::getBufferPtr(int buffer_nb, int concat_frame_nb)
{
	return m_buffer_mgr.getBufferPtr(buffer_nb, concat_frame_nb);
}


void *BufferCtrlObj::getFramePtr(int acq_frame_nb)
{
	return m_buffer_mgr.getFramePtr(acq_frame_nb);
}


void BufferCtrlObj::getStartTimestamp(Timestamp &start_ts)
{
	m_buffer_mgr.getStartTimestamp(start_ts);
}


void BufferCtrlObj::getFrameInfo(int acq_frame_nb, HwFrameInfoType &info)
{
	m_buffer_mgr.getFrameInfo(acq_frame_nb, info);
}


void BufferCtrlObj::registerFrameCallback(HwFrameCallback &frame_cb)
{
	m_buffer_mgr.registerFrameCallback(frame_cb);
}


void BufferCtrlObj::unregisterFrameCallback(HwFrameCallback &frame_cb)
{
	m_buffer_mgr.unregisterFrameCallback(frame_cb);
}


/***************************************************************//**
 * @brief Espia::Focla::SyncCtrlObj class constructor
 *
 * @param[in] acq          reference to Espia::Acq object
 * @param[in] focla        reference to Espia::Focla::Dev object
 *******************************************************************/
SyncCtrlObj::SyncCtrlObj( Espia::Acq &acq, Dev &focla)
	: HwSyncCtrlObj(), m_acq(acq), m_focla(focla)
{
}


SyncCtrlObj::~SyncCtrlObj()
{
}


bool SyncCtrlObj::checkTrigMode(TrigMode trig_mode)
{
	return (trig_mode == IntTrig);
}


void SyncCtrlObj::setTrigMode(TrigMode trig_mode)
{
	if (!checkTrigMode(trig_mode))
		throw LIMA_HW_EXC(InvalidValue, "Invalid (external) trigger");
}


void SyncCtrlObj::getTrigMode(TrigMode &trig_mode)
{
	trig_mode = IntTrig;
}


void SyncCtrlObj::setExpTime(double exp_time)
{
}


void SyncCtrlObj::getExpTime(double &exp_time)
{
}


void SyncCtrlObj::setLatTime(double lat_time)
{
}


void SyncCtrlObj::getLatTime(double &lat_time)
{
}


void SyncCtrlObj::setNbHwFrames(int nb_frames)
{
	m_acq.setNbFrames(nb_frames);
}


void SyncCtrlObj::getNbHwFrames(int &nb_frames)
{
	m_acq.getNbFrames(nb_frames);
}


void SyncCtrlObj::getValidRanges(ValidRangesType &valid_ranges)
{
}


/***************************************************************//**
 * @brief Espia::Focla::Interface class constructor
 *
 * @param[in] acq          reference to Espia::Acq object
 * @param[in] buffer_mgr   reference to BufferCtrlMgr object
 * @param[in] focla        reference to Espia::Focla::Dev object
 *******************************************************************/
Interface::Interface( Espia::Acq &acq, BufferCtrlMgr &buffer_mgr, 
                      Espia::Focla::Dev &focla )
	: m_acq(acq), m_buffer_mgr(buffer_mgr), m_focla(focla),
	  m_det_info(focla), m_buffer(buffer_mgr), 
	  m_sync(acq, focla)
{
	HwDetInfoCtrlObj *det_info = &m_det_info;
	m_cap_list.push_back(HwCap(det_info));

	HwBufferCtrlObj *buffer = &m_buffer;
	m_cap_list.push_back(HwCap(buffer));

	HwSyncCtrlObj *sync = &m_sync;
	m_cap_list.push_back(HwCap(sync));

	reset(SoftReset);
}


Interface::~Interface()
{
}


void Interface::getCapList(HwInterface::CapList &cap_list) const
{
	cap_list = m_cap_list;
}


void Interface::reset(ResetLevel reset_level)
{
	m_acq.stop();

	if( reset_level == HardReset )  // ???
		m_focla.setParam( TEST_IMAGE, 0 );

	m_sync.setNbFrames(1);

	m_buffer.setNbConcatFrames(1);
//	m_buffer.setNbBuffers(1);  // We need to set the FrameDim before this!
}


void Interface::prepareAcq()
{
}


void Interface::startAcq()
{
	m_acq.start();
	m_focla.setParam( TEST_IMAGE, 1 );
}


void Interface::stopAcq()
{
	m_focla.setParam( TEST_IMAGE, 0 );
	m_acq.stop();
}


void Interface::getStatus(StatusType& status)
{
	Acq::Status acq;
	m_acq.getStatus(acq);
	status.acq = acq.running ? AcqRunning : AcqReady;

	static const DetStatus det_mask = DetIdle;  // ???
	status.det_mask = det_mask;
}


int Interface::getNbHwAcquiredFrames()
{
	Acq::Status acq_status;
	m_acq.getStatus(acq_status);
	return acq_status.last_frame_nb + 1;
}
