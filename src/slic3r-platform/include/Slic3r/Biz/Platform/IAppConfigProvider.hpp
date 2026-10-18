#pragma once

#include <boost/filesystem/path.hpp>

#include <string>
#include <utility>

namespace Slic3r::Biz::Platform {

class IAppConfigProvider {
public:
  virtual ~IAppConfigProvider() = default;
  virtual boost::filesystem::path download_dir() const = 0;

  virtual std::string last_used_physical_printer() const = 0;
  virtual void set_last_used_physical_printer(const std::string& uuid) = 0;

  /// Returns {hw_config_key, preset_id} of the printer preset selected last time, or empty strings.
  /// The hw config key identifies the printer model across restarts (its runtime id does not).
  virtual std::pair<std::string, std::string> last_used_printer_preset() const = 0;
  virtual void set_last_used_printer_preset(const std::string& hw_config_key, const std::string& preset_id) = 0;

  virtual bool get_show_step_import_parameters() const = 0;
  virtual void set_show_step_import_parameters(bool show) = 0;
  virtual double get_step_linear_precision() const = 0;
  virtual void set_step_linear_precision(double precision) = 0;
  virtual double get_step_angle_precision() const = 0;
  virtual void set_step_angle_precision(double precision) = 0;
};

} // namespace Slic3r::Biz::Platform
