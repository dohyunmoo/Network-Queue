#include "class.cpp"
#include <cmath>
#include <time.h>

int queue(float r, unsigned int q, unsigned int tot_time);
float event_generator(float input);
int looper();

int main(int argc, char *argv[]) {
    float rho = 0.75;
    unsigned int queue_size = 25;
    unsigned int total_event_time = 1000;

    for (std::size_t i = 1; i < argc; i++) {
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
    srand((unsigned)time(NULL));
    float i = (float) rand()/RAND_MAX;
    return -(1.0 / input) * log( 1.0 - (float)rand()/RAND_MAX);
}

int queue(float r, unsigned int q, unsigned int tot_time) {
    double * inter_arrival;
    double * arrival;
    double * service;
    double * inter_observer;
    double * observer;
    double * departure;

    return 0;
}
