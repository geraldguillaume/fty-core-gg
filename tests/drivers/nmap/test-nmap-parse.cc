/*
 * tests for nmap-parse
 */

#include <catch.hpp>
#include <czmq.h>
#include "nmap-parse.h"
#include "nmap_msg.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <tuple>
#include <cstdint>

#ifndef TDIR
#error TDIR is not defined, build with -DTDIR=/path/to/tests
#endif

#define __xstr(a) __str(a)
#define __str(a) #a
#define TEST_DIR __xstr(TDIR)

static const char* TEST_DRIVER_NMAP_REPLY = "ipc://@/bios/TESTS/driver/nmap_reply";

static const std::vector<std::string> MBT_LAB_DATA = {
        "10.130.32.2",
        "10.130.32.6",
        "10.130.32.11",
        "10.130.32.49",
        "10.130.32.50",
        "10.130.32.51",
        "10.130.32.52",
        "10.130.32.53",
        "10.130.32.54",
        "10.130.32.55",
        "10.130.32.56",
        "10.130.32.57",
        "10.130.32.58",
        "10.130.32.59",
        "10.130.32.60",
        "10.130.32.61",
        "10.130.32.62",
        "10.130.32.63",
        "10.130.32.64",
        "10.130.32.65",
        "10.130.32.70",
        "10.130.32.71",
        "10.130.32.72",
        "10.130.32.73",
        "10.130.32.74",
        "10.130.32.76",
        "10.130.32.79",
        "10.130.32.80",
        "10.130.32.83",
        "10.130.32.87",
        "10.130.32.90",
        "10.130.32.91",
        "10.130.32.92",
        "10.130.32.94",
        "10.130.32.100",
        "10.130.32.101",
        "10.130.32.102",
        "10.130.32.105",
        "10.130.32.106",
        "10.130.32.111",
        "10.130.32.112",
        "10.130.32.113",
        "10.130.32.114",
        "10.130.32.115",
        "10.130.32.117",
        "10.130.32.118",
        "10.130.32.120",
        "10.130.32.121",
        "10.130.32.122",
        "10.130.32.123",
        "10.130.32.124",
        "10.130.32.125",
        "10.130.32.131",
        "10.130.32.132",
        "10.130.32.133",
        "10.130.32.134",
        "10.130.32.135",
        "10.130.32.136",
        "10.130.32.137",
        "10.130.32.138",
        "10.130.32.143",
        "10.130.32.144",
        "10.130.32.145",
        "10.130.32.146",
        "10.130.32.177",
        "10.130.32.178",
        "10.130.32.179",
        "10.130.32.180",
        "10.130.32.181",
        "10.130.32.182",
        "10.130.32.183",
        "10.130.32.184",
        "10.130.32.185",
        "10.130.32.186",
        "10.130.32.187",
        "10.130.33.0",
        "10.130.33.2",
        "10.130.33.3",
        "10.130.33.4",
        "10.130.33.5",
        "10.130.33.6",
        "10.130.33.10",
        "10.130.33.11",
        "10.130.33.12",
        "10.130.33.24",
        "10.130.33.27",
        "10.130.33.39",
        "10.130.33.40",
        "10.130.33.52",
        "10.130.33.53",
        "10.130.33.54",
        "10.130.33.55",
        "10.130.33.56",
        "10.130.33.58",
        "10.130.33.59",
        "10.130.33.60",
        "10.130.33.61",
        "10.130.33.62",
        "10.130.33.63",
        "10.130.33.64",
        "10.130.33.65",
        "10.130.33.66",
        "10.130.33.67",
        "10.130.33.68",
        "10.130.33.69",
        "10.130.33.70",
        "10.130.33.71",
        "10.130.33.72",
        "10.130.33.73",
        "10.130.33.74",
        "10.130.33.75",
        "10.130.33.77",
        "10.130.33.78",
        "10.130.33.79",
        "10.130.33.81",
        "10.130.33.82",
        "10.130.33.83",
        "10.130.33.84",
        "10.130.33.86",
        "10.130.33.87",
        "10.130.33.90",
        "10.130.33.91",
        "10.130.33.92",
        "10.130.33.95",
        "10.130.33.96",
        "10.130.33.97",
        "10.130.33.98",
        "10.130.33.99",
        "10.130.33.100",
        "10.130.33.101",
        "10.130.33.102",
        "10.130.33.103",
        "10.130.33.104",
        "10.130.33.105",
        "10.130.33.106",
        "10.130.33.107",
        "10.130.33.147",
        "10.130.33.149",
        "10.130.33.181",
        "10.130.33.184",
        "10.130.33.201",
        "10.130.33.204",
        "10.130.33.211",
        "10.130.33.212",
        "10.130.33.213",
        "10.130.33.216",
        "10.130.33.217",
        "10.130.33.218",
        "10.130.33.225",
        "10.130.33.226",
        "10.130.33.227",
        "10.130.33.231",
        "10.130.33.233",
        "10.130.33.234",
        "10.130.33.235",
        "10.130.33.236",
        "10.130.33.237",
        "10.130.34.2",
        "10.130.34.3",
        "10.130.34.5",
        "10.130.34.6",
        "10.130.34.7",
        "10.130.34.8",
        "10.130.34.9",
        "10.130.34.11",
        "10.130.34.12",
        "10.130.34.17",
        "10.130.34.18",
        "10.130.34.19",
        "10.130.34.20",
        "10.130.34.21",
        "10.130.34.22",
        "10.130.34.23",
        "10.130.34.25",
        "10.130.34.26",
        "10.130.34.27",
        "10.130.34.34",
        "10.130.34.40",
        "10.130.34.41",
        "10.130.34.42",
        "10.130.34.43",
        "10.130.34.46",
        "10.130.34.50",
        "10.130.34.52",
        "10.130.34.54",
        "10.130.34.57",
        "10.130.34.63",
        "10.130.34.68",
        "10.130.34.69",
        "10.130.34.72",
        "10.130.34.73",
        "10.130.34.74",
        "10.130.34.78",
        "10.130.34.93",
        "10.130.34.98",
        "10.130.34.101",
        "10.130.34.104",
        "10.130.34.122",
        "10.130.34.123",
        "10.130.34.124",
        "10.130.34.126",
        "10.130.34.127",
        "10.130.34.131",
        "10.130.34.132",
        "10.130.34.134",
        "10.130.34.137",
        "10.130.34.143",
        "10.130.34.157",
        "10.130.34.159",
        "10.130.34.160",
        "10.130.34.164",
        "10.130.34.166",
        "10.130.34.172",
        "10.130.34.180",
        "10.130.34.181",
        "10.130.34.185",
        "10.130.34.190",
        "10.130.34.198",
        "10.130.34.200",
        "10.130.34.209",
        "10.130.34.211",
        "10.130.34.232",
        "10.130.34.233",
        "10.130.34.238",
        "10.130.34.239",
        "10.130.34.240",
        "10.130.34.247",
        "10.130.34.249",
        "10.130.34.250",
        "10.130.35.139",
        "10.130.35.229",
        "10.130.35.230",
        "10.130.35.231",
        "10.130.35.232",
        "10.130.35.233",
        "10.130.35.234",
        "10.130.35.239",
        "10.130.35.240",
        "10.130.35.241",
        "10.130.35.242",
        "10.130.35.243",
        "10.130.35.244",
        "10.130.35.247",
        "10.130.35.249",
        "10.130.35.252"
};

const static std::string s_read_file(const char* filename) {
    //FIXME: that's ugly, figure out how to make it better
    std::string path{TEST_DIR};
    path += "/";
    path += filename;
    std::ifstream inpf{path};

    std::stringstream strStream;
    strStream << inpf.rdbuf();
    inpf.close();
    const std::string ret = strStream.str();
    REQUIRE(ret != "");
    return ret;
}

TEST_CASE("nmap-parse-mbt-lab", "[driver-nmap]") {
    auto s = s_read_file("mbt-lab.xml");
    zsock_t *insock = zsock_new_router(TEST_DRIVER_NMAP_REPLY);
    REQUIRE(insock != NULL);
    zclock_sleep(150);
    zsock_t *ousock = zsock_new_dealer(TEST_DRIVER_NMAP_REPLY);
    REQUIRE(ousock != NULL);
    parse_list_scan(s, ousock);

    for (auto i=0u; i != MBT_LAB_DATA.size(); i++) {
        nmap_msg_t *msg = nmap_msg_recv(insock);
        CHECK(msg != NULL);
        CHECK(nmap_msg_id(msg) == NMAP_MSG_LIST_SCAN);
        CHECK(nmap_msg_addr(msg) == MBT_LAB_DATA[i]);
        //TODO: we should test the hostnames data
        CHECK(nmap_msg_hostnames_size(msg) == 1);
        nmap_msg_destroy(&msg);
    }
    nmap_msg_t *msg = nmap_msg_recv_nowait(insock);
    REQUIRE(msg == NULL);
    zsock_destroy(&insock);
    zsock_destroy(&ousock);
}