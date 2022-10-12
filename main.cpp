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

    std::vector<Events> total; // total events buffer
    std::vector<Events> buffer; // temporary buffer used to sort
    std::vector<ObserverData> observer_list; // temporary buffer used to sort

    unsigned int transmission_rate = 1000000;
    unsigned int average_bit_length = 2000;
    unsigned int arrival_count = 0;
    unsigned int departure_count = 0;
    unsigned int observer_count = 0;
    unsigned int packet_loss_count = 0;

    double packet_loss_prob = 0;
    double queue_idle_prob = 0;
    double average_packet_count = 0;
    unsigned int empty_count = 0;

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

    unsigned int i = 0;
    unsigned int j = 0;
    
    while (i < arrival.size() && j < departure.size()) {
        if (arrival[i] <= departure[j]) {
            buffer.push_back(Events("Arrival", arrival[i]));
            i++;
        } else {
            buffer.push_back(Events("Departure", departure[j]));
            j++;
        }
    }

    while (i < arrival.size()) {
        buffer.push_back(Events("Arrival", arrival[i]));
        i++;
    }

    while (j < departure.size()) {
        buffer.push_back(Events("Departure", departure[j]));
        j++;
    }

    unsigned int k = 0;
    unsigned int l = 0;

    while (k < buffer.size() && l < observer.size()) {
        if (buffer[k].get_time() <= observer[l]) {
            total.push_back(Events(buffer[k].get_event(), buffer[k].get_time()));
            k++;
        } else {
            total.push_back(Events("Observer", observer[l]));
            l++;
        }
    }

    while (k < buffer.size()) {
        total.push_back(Events(buffer[k].get_event(), buffer[k].get_time()));
        k++;
    }

    while (l < observer.size()) {
        total.push_back(Events("Observer", observer[l]));
        l++;
    }

    for (size_t index = 0; index < total.size(); index++) {
        if (q == 0) {
            if (total[index].get_event() == "Arrival") {
                arrival_count++;
            }
        } else {
            if (total[index].get_event() == "Arrival") {
                if (arrival_count - departure_count < q) {
                    arrival_count++;
                } else {
                    packet_loss_count++;
                }
            }
        }

        if (total[index].get_event() == "Departure") {
            departure_count++;
        } else if (total[index].get_event() == "Observer") {
            observer_count++;
            observer_list.push_back(ObserverData(arrival_count, departure_count, observer_count));
        }
    }

    unsigned int current_sum = 0;
    for (size_t index = 0; index < observer_list.size(); index++) {
        if (observer_list[index].get_arrival() - observer_list[index].get_departure() == 0) {
            empty_count++;
        }
        current_sum += observer_list[index].get_arrival() - observer_list[index].get_departure();
    }

    packet_loss_prob = packet_loss_count / (arrival_count + packet_loss_count);
    average_packet_count = current_sum / observer_count;
    queue_idle_prob = empty_count / observer_count;

    std::cout << "rho: " << r << ", queue size: " << q << ", total simulation time: " << tot_time << std::endl;
    std::cout << "packet loss probability: " << packet_loss_prob << std::endl;
    std::cout << "average packet count: " << average_packet_count << std::endl;
    std::cout << "queue idle probability: " << queue_idle_prob << std::endl;

    return 0;
}
