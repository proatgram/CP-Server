/*
MIT License

Copyright (c) 2022 Timothy Hutchins

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "main.h"

void handleGET(web::http::http_request request, MagicPacket& magic) {
    const utility::string_t qs = request.request_uri().query();
    auto q = web::uri::split_query(qs);

    if (q.find(POWER_ON) != q.end()) {
        magic.send();
        std::cout << "Sent wake up packet." << std::endl;
        request.reply(web::http::status_codes::OK);
        return;
    }

    request.reply(web::http::status_codes::NoContent);
}


int main(int argc, char **argv) {

    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <mac address> <interface device>" << std::endl;
        exit(EX_USAGE);
    }

    web::http::experimental::listener::http_listener listener("http://localhost:5555");

    MagicPacket magic(argv[1], argv[2]);

    listener.support(web::http::methods::GET, [&magic](web::http::http_request request) {handleGET(request, magic);});

    listener.open().wait();

    while (true) {

    }

    return EXIT_SUCCESS;
}
