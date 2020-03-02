#ifndef _LINUX_SERIAL_CTRL_H_
#define _LINUX_SERIAL_CTRL_H_

#include <ctime>
#include <cstdint>
#include <cstring>
#include <cerrno>

#include <fcntl.h>             // Contains file controls like O_RDWR
#include <errno.h>             // Error integer and strerror() function
#include <termios.h>           // Contains POSIX terminal control definitions
#include <unistd.h>            // write(), read(), close()
#include <sys/ioctl.h> 
#include <linux/serial.h> 


#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

#include "num2string.h"

//-----------------------------------------------------------------------------
class serial_port
{


private:
    //int port;
    struct termios settings;

//-----------------------------------------------------------------------------
    void config(uint32_t baud_rate, uint32_t wait_time)
    {
        struct serial_struct serial; 
        //ioctl(fd, TIOCGSERIAL, &serial); 
        
        //serial.flags |= ASYNC_LOW_LATENCY; 
        
        //serial.xmit_fifo_size = 20;
        //ioctl(fd, TIOCSSERIAL, &serial); 
        
        
        // get the current port configuration
        tcgetattr(port, &settings);

        // set the baud rate
        cfsetispeed(&settings, baud_rate);
        cfsetospeed(&settings, baud_rate);

        settings.c_cflag &= ~PARENB;        // No Parity
        settings.c_cflag &= ~CSTOPB;        // Stop bits = 1
        settings.c_cflag &= ~CSIZE;            // Clears the Mask
        settings.c_cflag |=  CS8;            // Set the data bits = 8

        // Turn off hardware based flow control (RTS/CTS).
        settings.c_cflag &= ~CRTSCTS;

        // Turn on the receiver of the serial port (CREAD), other wise reading from the serial port will not work.
        settings.c_cflag |= CREAD | CLOCAL;

        // Turn off software based flow control (XON/XOFF).
        settings.c_iflag &= ~(IXON | IXOFF | IXANY);

        // Setting the mode of operation,the default mode of operation of serial port in
        // Linux is the Cannonical mode. For Serial communications with outside devices
        // like serial modems, mice etc NON Cannonical mode is recommended.
        settings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);

        // Prevent special interpretation of output bytes (e.g. newline chars)
        settings.c_oflag &= ~(OPOST | ONLCR);

        // set wait time
        settings.c_cc[VTIME] = wait_time;    // Wait for up to 1s (100ms increments), returning as soon as any data is received.
        settings.c_cc[VMIN] = 1;

        // Save tty settings, also checking for error
        int res = tcsetattr(port, TCSANOW, &settings);
        if (res != 0) 
            throw std::runtime_error("Error from tcsetattr: " + std::string(strerror(errno)));

    }

//-----------------------------------------------------------------------------
public:
    int port;

    serial_port() = default;

//-----------------------------------------------------------------------------
    void open_port(std::string named_port, uint32_t baud_rate, uint32_t wait_time)
    {
        //port = open(named_port.c_str(), O_RDWR | O_NOCTTY);
        port = open(named_port.c_str(), O_RDWR | O_NOCTTY);
        
        if(port == 1)
        {
            throw std::runtime_error("Error opening port: " + named_port);
            return;
        }

        config(baud_rate, wait_time);
        
        flush_port();

    }

//-----------------------------------------------------------------------------
    uint64_t read_port(std::vector<char> &read_bufffer, uint64_t count)
    {
        uint64_t idx;
        uint64_t bytes_read = 0;
        read_bufffer.clear();
        read_bufffer.resize(count);

        for (idx = 0; idx < count; ++idx)
        {
            bytes_read += read(port, &read_bufffer[idx], 1);
        }

        if(bytes_read != count)
        {
            //throw std::runtime_error("Wrong number of bytes received. Expected: " + num2str(count,"%d") + ", received: " + num2str(num_bytes,"%d"));
            //return;
            std::cout << "Wrong number of bytes received. Expected: " << num2str(count, "%d") << ", received: " << num2str(bytes_read, "%d") << std::endl;
        }
        return bytes_read;
    }


    int64_t read_port(std::vector<uint8_t> &read_bufffer, uint64_t count)
    {
        uint64_t idx;
        uint64_t bytes_read = 0;
        read_bufffer.clear();
        read_bufffer.resize(count+1);

        for (idx = 0; idx < count; ++idx)
        {
            bytes_read += read(port, &read_bufffer[idx], 1);
        }

        if(bytes_read != count)
        {
            //throw std::runtime_error("Wrong number of bytes received. Expected: " + num2str(count,"%d") + ", received: " + num2str(num_bytes,"%d"));
            //return;
            std::cout << "Wrong number of bytes received. Expected: " << num2str(count, "%d") << ", received: " << num2str(bytes_read, "%d") << std::endl;
        }
        return bytes_read;
    }

    int64_t read_port(std::string &read_bufffer, uint64_t count)
    {
        std::vector<uint8_t> rb(count);

        int64_t num_bytes = read_port(rb, count);

        read_bufffer.assign(rb.begin(), rb.end());

        return num_bytes;
    }

//-----------------------------------------------------------------------------
    int64_t write_port(std::vector<char> write_buffer)
    {
        uint64_t write_size = write_buffer.size();
        int64_t bytes_written = write(port, write_buffer.data(), write_size);
        return bytes_written;
    }

    int64_t write_port(std::vector<uint8_t> write_buffer)
    {
        uint64_t write_size = write_buffer.size();
        int64_t bytes_written = write(port, write_buffer.data(), write_size);
        return bytes_written;
    }

    int64_t write_port(std::string write_buffer)
    {
        uint64_t write_size = write_buffer.length();
        int64_t bytes_written = write(port, write_buffer.c_str(), write_size);
        return bytes_written;
    }

//-----------------------------------------------------------------------------
    void flush_port()
    {
        usleep(2);
        tcflush(port, TCIOFLUSH);
    }
    
    inline int64_t bytes_available()
    {
        int64_t bytes_avail = 0;
        ioctl(port, FIONREAD, &bytes_avail);
        return bytes_avail;
    }

//-----------------------------------------------------------------------------
    void close_port()
    {
        close(port);
        port = 0;
    }

};    // end of class

#endif    // _LINUX_SERIAL_CTRL_H_
