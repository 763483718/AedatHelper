#pragma once

#include <fstream>
#include <iostream>
#include <vector>

#include "aedat.hpp"

namespace dvs_gesture {
struct DataSet {
  struct Row {
    uint32_t label;
    uint32_t startTime;
    uint32_t endTime;
  };

  struct DataPoint {
    uint32_t label;
    std::vector<AEDAT::PolarityEvent> events;
  };

  void load(const std::string &aedat_filename,
            const std::string &labels_filename) {

    std::fstream fs;
    char line[256];

    fs.open(labels_filename, std::fstream::in);

    if(!fs)
    {
      std::cout << "error in opening the file:\t" << labels_filename << std::endl;
      return;
    }

    fs.getline(line, 256);
    std::vector<Row> rows;

    // std::cout << __func__ << " begin to read csv" << std::endl;

    while (!fs.eof()) {
      Row row;
      fs >> row.label;
      fs.ignore(1);
      fs >> row.startTime;
      fs.ignore(1);
      fs >> row.endTime;
      fs.ignore(1);
      rows.push_back(row);
    }
    rows.pop_back();
    // std::cout << __func__ << " end read csv" << std::endl;

    AEDAT data;
    // std::cout << __func__ << " begin to load" << std::endl;
    data.load(aedat_filename);
    // std::cout << __func__ << " end load" << std::endl;

    size_t event_idx = 0;
    for (size_t row_idx = 0; row_idx < rows.size(); row_idx++) {
      auto datapoint = DataPoint{rows[row_idx].label};

      while (data.polarity_events[event_idx].timestamp <
             rows[row_idx].startTime) {
        event_idx++;
      }

      while (data.polarity_events[event_idx].timestamp <
             rows[row_idx].endTime) {
        auto event = data.polarity_events[event_idx];
        event.timestamp -= rows[row_idx].startTime;
        datapoint.events.push_back(event);
        event_idx++;
      }

      datapoints.push_back(datapoint);
    }
  }

  DataSet(const std::string &aedat_filename,
          const std::string &labels_filename) {
    load(aedat_filename, labels_filename);
  }

  DataSet() {}

  std::vector<DataPoint> datapoints;
};
} // namespace dvs_gesture
