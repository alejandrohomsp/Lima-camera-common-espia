#include "Espia.h"
#include "MemUtils.h"

using namespace lima;
using namespace std;

#define CHECK_CALL(ret)		ESPIA_CHECK_CALL(ret)

#define ESPIA_MIN_BUFFER_SIZE	(128 * 1024)


Espia::Espia(int dev_nr)
{
	m_dev_nr = Invalid;
	m_dev = ESPIA_DEV_INVAL;

	m_nb_buffers = m_buffer_frames = 0;
	m_real_frame_factor = m_real_frame_size = 0;
	
	open(dev_nr);
}

Espia::~Espia()
{
	close();
}

void Espia::open(int dev_nr)
{
	if (dev_nr == m_dev_nr)
		return;

	close();

	CHECK_CALL(espia_open(dev_nr, &m_dev));
	m_dev_nr = dev_nr;
}

void Espia::close()
{
	if (m_dev_nr == Invalid)
		return;

	bufferFree();

	CHECK_CALL(espia_close(m_dev));

	m_dev = ESPIA_DEV_INVAL;
	m_dev_nr = Invalid;
}

void Espia::bufferAlloc(const FrameDim& frame_dim, int& nb_buffers,
			int buffer_frames)
{
	if (!frame_dim.isValid() || (nb_buffers <= 0) || (buffer_frames <= 0))
		throw LIMA_HW_EXC(InvalidValue, "Invalid frame_dim, "
				  "nb_buffers and/or buffer_frames");

	if ((frame_dim == m_frame_dim) && (nb_buffers == m_nb_buffers) &&
	    (buffer_frames == m_buffer_frames))
		return;

	bufferFree();

	int& virt_buffers   = nb_buffers;
	int& virt_frames    = buffer_frames;

	int real_buffers    = virt_buffers;
	int real_frames     = virt_frames;
	int real_frame_size = frame_dim.getMemSize();
	if (virt_frames == 1) {
		int page_size;
		GetPageSize(page_size);
		real_frame_size += page_size - 1;
		real_frame_size &= ~(page_size - 1);
	}

	int real_buffer_size = real_frame_size * real_frames;
	int frame_factor = 1;
	if (real_buffer_size < ESPIA_MIN_BUFFER_SIZE) {
		frame_factor  = ESPIA_MIN_BUFFER_SIZE / real_buffer_size;
		real_frames  *= frame_factor;
		real_buffers += frame_factor - 1;
		real_buffers /= frame_factor;
		virt_buffers  = real_buffers * frame_factor;
	}

	CHECK_CALL(espia_buffer_alloc(m_dev, real_buffers, real_frames,
				      real_frame_size));

	m_frame_dim         = frame_dim;
	m_nb_buffers        = virt_buffers;
	m_buffer_frames     = virt_frames;
	m_real_frame_factor = frame_factor;
	m_real_frame_size   = real_frame_size;
}

void Espia::bufferFree()
{
	if ((m_nb_buffers == 0) || (m_buffer_frames == 0))
		return;

	CHECK_CALL(espia_buffer_free(m_dev));

	m_frame_dim = FrameDim();
	m_nb_buffers = m_buffer_frames = 0;
	m_real_frame_factor = m_real_frame_size = 0;
}

void Espia::getFrameDim(FrameDim& frame_dim)
{
	frame_dim = m_frame_dim;
}

void Espia::getNbBuffers(int& nb_buffers)
{
	nb_buffers = m_nb_buffers;
}

void Espia::getBufferFrames(int& buffer_frames)
{
	buffer_frames = m_buffer_frames;
}

void *Espia::getBufferFramePtr(int buffer_nb, int frame_nb)
{
	int real_buffer = realBufferNb(buffer_nb, frame_nb);
	int real_frame  = realFrameNb(buffer_nb, frame_nb);
	void *ptr;
	CHECK_CALL(espia_frame_address(m_dev, real_buffer, real_frame, &ptr));
	return ptr;
}

void *Espia::getAcqFramePtr(int acq_frame_nb)
{
	int buffer_nb = ESPIA_ACQ_ANY;
	void *ptr;
	CHECK_CALL(espia_frame_address(m_dev, buffer_nb, acq_frame_nb, &ptr));
	return ptr;
}

void Espia::getFrameInfo(int acq_frame_nb, HwFrameInfoType& info)
{
	struct img_frame_info finfo;
	finfo.buffer_nr    = ESPIA_ACQ_ANY;
	finfo.frame_nr     = ESPIA_ACQ_ANY;
	finfo.round_count  = ESPIA_ACQ_ANY;
	finfo.acq_frame_nr = acq_frame_nb;
	CHECK_CALL(espia_get_frame(m_dev, &finfo, SCDXIPCI_NO_BLOCK));

	real2virtFrameInfo(finfo, info);
}

void Espia::real2virtFrameInfo(const struct img_frame_info& real_info, 
			       HwFrameInfoType& virt_info)
{
	char *buffer_ptr = (char *) real_info.buffer_ptr;
	int frame_offset = real_info.frame_nr * m_real_frame_size;

	virt_info.acq_frame_nb    = real_info.acq_frame_nr;
	virt_info.frame_ptr       = buffer_ptr + frame_offset;
	virt_info.frame_dim       = &m_frame_dim;
	virt_info.frame_timestamp = usec2sec(real_info.time_us);
	virt_info.valid_pixels    = real_info.pixels;
}

void Espia::serWrite(const string& buffer, int block_size, double block_delay, 
		     bool no_wait)
{
	unsigned long len = buffer.length();
	char *ptr = len ? (char *) buffer.data() : NULL;
	CHECK_CALL(espia_ser_write(m_dev, ptr, &len, block_size, 
				   sec2usec(block_delay), !no_wait));
}

void Espia::serRead(string& buffer, int len, double timeout)
{
	buffer.resize(len);
	char *ptr = len ? (char *) buffer.data() : NULL;
	unsigned long ret_len = len;
	CHECK_CALL(espia_ser_read(m_dev, ptr, &ret_len, sec2usec(timeout)));
	buffer.resize(ret_len);
}

void Espia::serReadStr(string& buffer, int len, const string& term, 
		       double timeout)
{
	buffer.resize(len);
	char *ptr = len ? (char *) buffer.data() : NULL;
	char *term_ptr = (char *) term.data();
	unsigned long ret_len = len;
	CHECK_CALL(espia_ser_read_str(m_dev, ptr, &ret_len, term_ptr,
				      term.length(), sec2usec(timeout)));
	buffer.resize(ret_len);
}

void Espia::serFlush()
{
	CHECK_CALL(espia_ser_flush(m_dev));
}
	
void Espia::throwError(int ret, string file, string func, int line)
{
	string err_desc = string("Espia: ") + espia_strerror(ret);
	throw Exception(Hardware, Error, err_desc, file, func, line);
}
