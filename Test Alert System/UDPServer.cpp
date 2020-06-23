#pragma warning(disable : 4996)

#include <iostream>
#include <fstream>

#include <chrono>
#include <iomanip>

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>



/*
Simple Listener On a UDP Port,
Will log all information received.
*/

using boost::asio::ip::udp;

const int PORT = 4210;

class udp_server
{
public:
    udp_server(boost::asio::io_service& io_service)
        : socket_(io_service, udp::endpoint(udp::v4(), PORT))
    {
        start_receive();

        std::string filename = "logfile--";


        filename += get_current_datetime(false);
        filename += ".txt";

        logfile.open(filename, std::ios::in | std::ios::ate | std::ios::app);

        if (logfile.is_open())
        {
            std::cout << "Opened file:" << filename << "\n";
        }
        else
        {
            std::cout << "Failure to open filename:" << filename << std::endl;
        }


    }


    


    ~udp_server()
    {
        std::cout << "Closing file..." << "\n";

        if (logfile.is_open())
        {
            logfile.close();
        }

    }


    std::string get_current_datetime(bool incsec)
    {

        timepoint = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(timepoint);
 
        std::stringstream ss;
 
        if (incsec)
        {
            ss << std::put_time(std::localtime(&time), "%Y-%m-%d %X");
        }
        else
        {
            ss << std::put_time(std::localtime(&time), "%Y-%m-%d");
        }

        std::cout << "DATETEST:" << ss.str() << "\n";
        return ss.str();
    }



private:
    void start_receive()
    {
        socket_.async_receive_from(
            boost::asio::buffer(recv_buffer_), remote_endpoint_,
            boost::bind(&udp_server::handle_receive, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }

    void handle_receive(const boost::system::error_code& error,
        std::size_t d/*bytes_transferred*/)
    {


        std::cout << "Received this many bytes from client:" << d << "\n";

        std::cout << "Data:" << recv_buffer_.data() << "\n";

        if (logfile.is_open())
        {
            std::cout << "File is open.\n";
            logfile << get_current_datetime(true) << " - " << recv_buffer_.data() << "\n";
            logfile.flush();
            recv_buffer_.empty();
        }
        else
        {
            std::cout << "File is closed.\n";
        }

        

        if (!error || error == boost::asio::error::message_size)
        {
            boost::shared_ptr<std::string> message(new std::string("Message Received."));

            socket_.async_send_to(boost::asio::buffer(*message), remote_endpoint_,
                                    boost::bind(&udp_server::handle_send, this, message,
                                    boost::asio::placeholders::error,
                                    boost::asio::placeholders::bytes_transferred));

            start_receive();
        }
    }

    void handle_send(boost::shared_ptr<std::string> /*message*/,
        const boost::system::error_code& /*error*/,
        std::size_t /*bytes_transferred*/)
    {
    }

    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    boost::array<char, 256> recv_buffer_;
    std::ofstream logfile;
    std::chrono::system_clock::time_point timepoint;

};




int main()
{
    try
    {
        boost::asio::io_service io_service;
        udp_server server(io_service);
        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;

}
