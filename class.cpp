#include <iostream>
#include <string>

class Events {
    private:
        std::string event_type;
        double time;
    public:
        Events(std::string event_value, double time_value) {
            event_type = event_value;
            time = time_value;
        }
        double get_time() {
            return time;
        }
        std::string get_event() {
            return event_type;
        }
};

class ObserverData {
    private:
        double arrival;
        double departure;
        double observer;
    public:
        ObserverData(double arrival_count, double departure_count, double observer_count) {
            arrival = arrival_count;
            departure = departure_count;
            observer = observer_count;
        }
        double get_arrival() {
            return arrival;
        }

        double get_departure() {
            return departure;
        }
        
        double get_observer() {
            return observer;
        }
};