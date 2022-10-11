#include "class.cpp"
#include <cmath>
#include <time.h>
#include <vector>
#include <ctime>

int queue(float r, unsigned int q, unsigned int tot_time);
float event_generator(float input);
int looper();

int main(int argc, char *argv[]) {
    std::srand((unsigned)std::time(NULL));

    float rho = 0.75;
    unsigned int queue_size = 0; // queue size default: infinite
    unsigned int total_event_time = 1000;

    for (size_t i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "-r") {
            rho = std::stof(argv[i + 1]);
        } else if (std::string(argv[i]) == "-q") {
            queue_size = (unsigned int)std::atoi(argv[i + 1]);
        } else if (std::string(argv[i]) == "-T") {
            total_event_time = (unsigned int)std::atoi(argv[i + 1]);
        }
    }

    queue(rho, queue_size, total_event_time);
    return 0;
}

float event_generator(float input) {
    return -(1.0 / input) * log( 1.0 - (float)rand()/RAND_MAX);
}

int queue(float r, unsigned int q, unsigned int tot_time) {
    std::vector<double> inter_arrival;
    std::vector<double> arrival;
    std::vector<double> service;
    std::vector<double> inter_observer;
    std::vector<double> observer;
    std::vector<double> departure;

    unsigned int transmission_rate = 1000000;
    unsigned int average_bit_length = 2000;
    unsigned int arrival_count = 0;
    unsigned int departure_count = 0;
    unsigned int observer_count = 0;
    unsigned int packet_loss_count = 0;

    double lambda = (r * transmission_rate) / average_bit_length;
    double service_rate = transmission_rate / (r * average_bit_length);
    double alpha = 5 * lambda;
    
    unsigned int a_index = 0;
    while (true) { // generating arrival times
        inter_arrival.push_back(event_generator(lambda));

        if (a_index == 0) {
            arrival.push_back(inter_arrival[a_index]);
        } else {
            if (arrival[a_index - 1] + inter_arrival[a_index] > tot_time) {
                break;
            } else {
                arrival.push_back(inter_arrival[a_index] + arrival[a_index - 1]);
            }
        }
        a_index++;
    }

    for (size_t i = 0; i < arrival.size(); i++) { // generating service times
        service.push_back(event_generator(service_rate));
    }

    unsigned int o_index = 0;
    while (true) { // generating observer times
        inter_observer.push_back(event_generator(alpha));

        if (o_index == 0) {
            observer.push_back(inter_observer[o_index]);
        } else {
            if (observer[o_index - 1] + inter_observer[o_index] > tot_time) {
                break;
            } else {
                observer.push_back(inter_observer[o_index] + observer[o_index - 1]);
            }
        }

        o_index++;
    }

    if (q == 0) {
        for (size_t i = 0; i < arrival.size(); i++) {
            if (i == 0 || arrival[i] >= departure[i - 1]) {
                departure.push_back(arrival[i] + service[i]);
            } else {
                departure.push_back(departure[i - 1] + service[i]);
            }
        }
    } else {
        unsigned int d_index = 0;

        for (size_t i = 0; i < arrival.size(); i++) {
            if (i < q) {
                if (i == 0 || arrival[i] >= departure[i - 1]) {
                    departure.push_back(arrival[i] + service[i]);
                } else {
                    departure.push_back(departure[i - 1] + service[i]);
                }
            } else {
                while (true) {
                    if (d_index == departure.size() - 1 || arrival[i] < departure[d_index]) {
                        break;
                    }
                    d_index++;
                }

                if (i - d_index < q) {
                    if (d_index == 0 || arrival[i] >= departure[d_index - 1]) {
                        departure.push_back(arrival[i] + service[i]);
                    } else {
                        departure.push_back(departure[d_index - 1] + service[i]);
                    }
                }
            }
        }
    }

    double* total;
    total = new double[arrival.size() + departure.size() + observer.size()];

    

    return 0;
}
