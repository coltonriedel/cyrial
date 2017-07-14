#ifndef CYRIAL_DEVICES_GPSDO_HPP
#define CYRIAL_DEVICES_GPSDO_HPP

#include <array>
#include <string>

#include "scpi.hpp"

namespace cyrial
{

enum sync_source { GPS, EXT, AUTO };

std::array<size_t, 5> gpsdo_baud{ 9600, 19200, 38400, 57600, 115200 };

/* @class gpsdo_device
 *
 * @brief Class to represent a GPS Disciplined Oscillator
 *
 * Implemented commands are influenced by the interfaces of Jackson Labs brand
 * GPSDO's, including:
 *   - FireFly IA OCXO
 *   - GPSTCXO
 *
 * TODO: ordering should be maintained for multiple messages, but scheduled
 * messages such as NEMA sentences can throw off these functions, resulting in
 * returning something that wasn't the intended command. Logic should be
 * implemented to verify the response (utilize command echo perhaps) and if
 * incorrect, store whatever response was returned into a buffer.
 * Corresponding functions to fetch NEMA, etc. message buffer contents should
 * also be added - note that NEMA messages start with '$', and command
 * responses are wrapped in an echo of the command and a newline
 */
class gpsdo_device : public scpi_device
{
public:
  /* @brief Constructor for gpsdo device
   *
   * @param A shared_ptr to a communication interface
   */
  gpsdo_device(std::shared_ptr<interface> port)
    : scpi_device(port)
  {
    comm->set_timeout(100);
    comm->set_baud(115200);
  }

  /* @brief Function to query the configuration, position, speed, height, and
   *        other relevant data of the integrated GPS receiver
   *
   * @return std::string Configuration, position, speed, height, and other
   *         relevant data
   */
  std::string gps()
  {
    return comm->query("GPS?");
  }

  /* @brief Function to query the number of tracked satellites
   *
   * TODO: convert to size_t
   *
   * @return std::string The number of tracked satellites
   */
  std::string gps_sat_tra_coun()
  {
    return comm->query("GPS:SAT:TRA:COUN?");
  }

  /* @brief Function to query the number of SV's which should be visible per the
   *        almanac
   *
   * TODO: convert to size_t
   *
   * @return std::string The number of visible SV's per the almanac
   */
  std::string gps_sat_vis_coun()
  {
    return comm->query("GPS:SAT:VIS:COUN?");
  }

  /* @brief Function to instruct the GPSDO to transmit GPGGA NEMA messages at a
   *        specified frequency (0:off)
   *
   * Note that this command is disabled during the first 4 minutes of GPSDO
   * operation
   *
   * @param freq The frequency in seconds (0,255) at which GPGGA NEMA messages
   *        should be output
   */
  void gps_gpgga(size_t freq)
  {
    if (freq <= 255)
    {
      comm->write("GPS:GPGGA " + std::to_string(freq));
      comm->eat();
    }
  }

  /* @brief Function to instruct the GPSDO to transmit modified GPGGA NEMA
   *        messages at a specified frequency (0:off)
   *
   * Messages differ from standard GPGGA messages in that they include the lock
   * state and health of the unit's **XO
   *
   * Note that this command is disabled during the first 7 minutes of GPSDO
   * operation
   *
   * @param freq The frequency in seconds (0,255) at which GPGGA NEMA messages
   *        should be output
   */
  void gps_ggast(size_t freq)
  {
    if (freq <= 255)
    {
      comm->write("GPS:GGAST " + std::to_string(freq));
      comm->eat();
    }
  }

  /* @brief Function to instruct the GPSDO to transmit GPRMC NEMA messages at a
   *        specified frequency (0:off)
   *
   * Note that this command is disabled during the first 4 minutes of GPSDO
   * operation
   *
   * @param freq The frequency in seconds (0,255) at which GPRMC NEMA messages
   *        should be output
   */
  void gps_gprmc(size_t freq)
  {
    if (freq <= 255)
    {
      comm->write("GPS:GPRMC " + std::to_string(freq));
      comm->eat();
    }
  }

  /* @brief Function to instruct teh GPSDO to transmit X, Y, and Z speed
   *        including centimeter-level accuracy estimates at a specified
   *        frequency
   *
   * Note that firmware version 0.909 or above is required to support this
   * command
   *
   * @param freq The frequency in seconds (0,255) at which GPRMC NEMA messages
   *        should be output
   */
  void gps_xyzsp(size_t freq)
  {
    if (freq <= 255)
    {
      comm->write("GPS:XYZSP " + std::to_string(freq));
      comm->eat();
    }
  }

  /* @brief Function to return information about time, including date, time in
   *        UTC, timezone, and time shift between the GPSDO and GPS time
   *
   * @return std::string Containing date, time (UTC), timezone, and time shift
   *         between the GPSDO and GPS time
   */
  std::string ptime()
  {
    return comm->query("PTIME?");
  }

  //Not supported on FireFly IA
  //
  ///* @brief Function to get the local timezone
  // *
  // * @return std::string The timezone of the receiver
  // */
  //std::string ptim_tzon()
  //{
  //  return comm->query("PTIM:TZON?");
  //}

  /* @brief Function to query the calendar date (UTC)
   *
   * TODO: should this return a date?
   *
   * @return std::string The calendar date (UTC) in year, month, day
   */
  std::string ptim_date()
  {
    return comm->query("PTIM:DATE?");
  }

  /* @brief Function to query the current time (UTC)
   *
   * TODO: return a time object
   *
   * @return std::string The current UTC time
   */
  std::string ptim_time()
  {
    return comm->query("PTIM:TIME?");
  }

  /* @brief Function to query the current time (UTC) in a format suitable for
   *        display (colon delimeters)
   *
   * @return std::string The current UTC time with colon delimeters
   */
  std::string ptim_time_str()
  {
    return comm->query("PTIM:TIME:STR?");
  }

  /* @brief Function to query the shift in GPSDO time from GPS time (1E-10
   *        seconds precision)
   *
   * Note that this function is equivalent to the @sync_tint function
   *
   * TODO: convert to numeric return value
   *
   * @return std::string The shift between GPSDO and GPS time
   */
  std::string ptim_tint()
  {
    return comm->query("PTIM:TINT?");
  }

  /* @brief Function to query the status of the synchronization system,
   *        including sync source, state, lock status, health, holdover
   *        duration, frequency error estimate, and the shift in GPSDO time
   *        from GPS time
   *
   * @return std::string The status of the synchronization system
   */
  std::string sync()
  {
    return comm->query("SYNC?");
  }

  /* @brief Function to set the 1 PPS source to be used for synchronization
   *        GPS  : internal GPS receiver
   *        EXT  : external 1 PPS source
   *        AUTO : use the internal receiver when available, fallback to EXT
   *
   * @param source The source to be used for synchronization
   */
  void sync_sour_mode(sync_source source)
  {
    switch (source)
    {
      case GPS:  comm->write("SYNC:SOUR:MODE GPS" ); comm->eat(); break;
      case EXT:  comm->write("SYNC:SOUR:MODE EXT" ); comm->eat(); break;
      case AUTO: comm->write("SYNC:SOUR:MODE AUTO"); comm->eat(); break;
    }
  }

  /* @brief Function to query the synchronization source being used
   *
   * TODO: return enum type
   *
   * @return std::string
   */
  std::string sync_sour_state()
  {
    return comm->query("SYNC:SOUR:STATE?");
  }

  /* @brief Function to query the length of the most recent holdover duration
   *
   * TODO: return time object
   *
   * @return std::string The length of the most recent holdover duration
   */
  std::string sync_hold_dur()
  {
    return comm->query("SYNC:HOLD:DUR?");
  }

  /* @brief Function to command the GPSDO to immediately enter holdover mode
   *
   */
  void sync_hold_init()
  {
    comm->write("SYNC:HOLD:INIT");
    comm->eat();
  }

  /* @brief Function to command terminate a manual holdover condition which
   *        was initiated through @sync_hold_init
   */
  void sync_hold_rec_init()
  {
    comm->write("SYNC:HOLD:REC:INIT");
    comm->eat();
  }

  /* @brief Function to query the shift in GPSDO time from GPS time (1E-10
   *        seconds precision)
   *
   * TODO: return a time object
   *
   * @return std::string The shift between GPSDO and GPS time
   */
  std::string sync_tint()
  {
    return comm->query("SYNC:TINT?");
  }

  /* @brief Function to command the GPSDO to synchronize with the reference
   *        1 PPS signal
   *
   * Note that this command is ignored when the oscillator is in holdover
   */
  void sync_imme()
  {
    comm->write("SYNC:IMME");
    comm->eat();
  }

  /* @brief Function to query the frequency error estimate
   *
   * Similar to the Allan variance, a 1000s interval is measured. Values below
   * 1E-12 are considered noise
   *
   * TODO: return a time object
   *
   * @return std::string The frequency error estimate
   */
  std::string sync_fee()
  {
    return comm->query("SYNC:FEE?");
  }

  /* @brief Function to query the lock status of the PLL which controls the
   *        oscillator
   *
   * TODO: return a bool
   *
   * @return std::string The lock status of the oscillator (0: OFF)
   */
  std::string sync_lock()
  {
    return comm->query("SYNC:LOCK?");
  }

  /* @brief Function to query the health status of the GPSDO
   *
   * 0x000 : healty and locked
   * 0x001 : OCXO course DAC maxed out at 255
   * 0x002 : OCXO course DAC min-ed out at 0
   * 0x004 : phase offset to UTC > 250ns
   * 0x008 : runtime < 300s
   * 0x010 : holdover > 60s
   * 0x020 : frequency error estimate out of bounds
   * 0x040 : OCXO voltage too high
   * 0x080 : OCXO voltage too low
   * 0x100 : short term (100s) drift > 100ns
   * 0x200 : runtime < 7min after phase-reset
   *
   * TODO: return hex value
   *
   * @return std::string The GPSDO health status
   */
  std::string sync_health()
  {
    return comm->query("SYNC:HEALTH?");
  }

  /* @brief Function to query the electronic frequency control value in percent
   *
   * TODO: return a float
   *
   * @return std::string The electronic frequency control value in percent
   */
  std::string diag_rosc_efc_rel()
  {
    return comm->query("DIAG:ROSC:EFC:REL?");
  }

  /* @brief Function to query the electronic frequency control value in volts
   *        (0 < v < 5)
   *
   * TODO: return a float
   *
   * @return std::string The electronic frequency control value in volts
   */
  std::string diag_rosc_efc_abs()
  {
    return comm->query("DIAG:ROSC:EFC:ABS?");
  }

  /* @brief Function to query the system status
   *
   * @return std::string Formatted system status
   */
  std::string syst_stat()
  {
    return comm->query("SYST:STAT?");
  }

  /* @brief Function to check if command echo is enabled on RS-232
   *
   * TODO: return boolean
   *
   * @return std::string Whether or not command echo is enabled
   */
  std::string syst_comm_ser_echo()
  {
    return comm->query("SYST:COMM:SER:ECHO?");
  }

  /* @brief Function to enable or disable command echo on RS-232
   *
   * This should not be disabled as it (TODO) is used to insure the correct
   * response is returned for a command
   */
  void syst_comm_ser_echo(bool state)
  {
    std::string command = "SYST:COMM:SER:ECHO " + state ? "ON" : "OFF";

    comm->write(command.c_str());
    comm->eat();
  }

  /* @brief Function to check of command prompt ("scpi>") is enabled
   *
   * TODO: return a bool
   *
   * @return std::string Whether or not command prompt is enabled
   */
  std::string syst_comm_ser_pro()
  {
    return comm->query("SYST:COMM:SER:PRO?");
  }

  /* @brief Function to enable or disable command prompt on RS-232
   *
   * This should not be disabled as it (TODO) is used to insure the correct
   * response is returned for a command
   */
  void syst_comm_ser_pro(bool state)
  {
    std::string command = "SYST:COMM:SER:PRO " + state ? "ON" : "OFF";

    comm->write(command.c_str());
    comm->eat();
  }

  /* @brief Function to query current baud rate setting for device
   *
   * TODO: return size_t
   *
   * @return std::string The current device setting for baud rate
   */
  std::string syst_comm_ser_baud()
  {
    return comm->query("SYST:COMM:SER:BAUD?");
  }

  /* @brief Function to change the baud rate for the device
   *
   * Proposed value must be in predefined list of rates. Default baud rate is
   * 115200. Note that the baud rate on the program side should also be adjusted
   * or communication will be lost
   *
   * TODO: should I automatically change device baud rate? Should changing baud
   *       rate on controller side attempt to send matching command?
   *
   * @param New value for baud rate
   */
  void syst_comm_ser_baud(size_t proposed)
  {
    for (size_t i = 0; i < gpsdo_baud.size(); ++i)
      if (proposed == gpsdo_baud[i])
      {
        comm->write("SYST:COMM:SER:BAUD");
        comm->eat();
        break;
      }
  }

  /* @brief Function to query the current settings of the servo loop
   *
   * @return std::string The parameters currently in use within the servo
   *         loop system
   */
  std::string serv()
  {
    return comm->query("SERV?");
  }

  /* @brief Function to set the course Dac which controls the EFC. Values should
   *        be in the range [0, 255]
   *
   * You should not need to use this function
   *
   * @param New value for coefficient
   */
  void serv_coarsd(size_t val)
  {
    if (val <= 255)
    {
      comm->write("SERV:COARSD " + std::to_string(val));
      comm->eat();
    }
  }

  /* @brief Function to set the proportional coefficient of the PID loop. Values
   *        are in range [0.0, 500.0]
   *
   * Larger values increase loop control at the expense of noise while locked.
   * Settings which are too high will cause instabilities.
   *
   * Typical values:
   *  0.7 : double oven OCXO
   *  6.0 : single oven OCXO
   *
   * @param New value for coefficient
   */
  void serv_efcs(double value)
  {
    if (value >= 0.0 && value <= 500.0)
    {
      comm->write("SERV:EFCS " + std::to_string(value));
      comm->eat();
    }
  }

  /* @brief Function to set the low pass filter effectiveness of the DAC. Values
   *        should be in range [0.0, 4000.0], and are typically in [2.0, 50.0]
   *
   * @param New value for coefficient
   */
  void serv_efcd(double value)
  {
    if (value >= 0.0 && value <= 4000.0)
    {
      comm->write("SERV:EFCD " + std::to_string(value));
      comm->eat();
    }
  }

  /* @brief Function to set the coefficient corresponding to the temperature
   *        compensation. Values should be in the range [-4000.0, 4000.0]
   *
   * @param New value for coefficient
   */
  void serv_tempco(double value)
  {
    if (value >= -4000.0 && value <= 4000.0)
    {
      comm->write("SERV:TEMPCO " + std::to_string(value));
      comm->eat();
    }
  }

  /* @brief Function to set the aging coefficient for the OCXO. Values should
   *        be in the range [-10.0, 10.0]
   *
   * @param New value for coefficient
   */
  void serv_aging(double value)
  {
    if (value >= -10.0 && value <= 10.0)
    {
      comm->write("SERV:AGING " + std::to_string(value));
      comm->eat();
    }
  }

  /* @brief Function to set the integral component of the PID loop. Values
   *        should be in the range [-100.0, 100.0]. Typical values are in the
   *        range [10.0, 30.0]
   *
   * A value which is too high will result in instability
   *
   * @param New value for coefficient
   */
  void serv_phaseco(double value)
  {
    if (value >= -100.0 && value <= 100.0)
    {
      comm->write("SERV:PHASECO " + std::to_string(value));
      comm->eat();
    }
  }

  /* @brief Function to query the GPSDO's offset to UTC
   *
   * TODO: convert return value to int
   *
   * @return the GPSDO's offset to UTC in nanoseconds
   */
  std::string serv_1pps()
  {
    return comm->query("SERV:1PPS?");
  }

  /* @brief Function to set the GPSDO's offset to UTC in 16.7ns incremnnts
   *
   * @param The new offset
   */
  void serv_1pps(int offset)
  {
    comm->write("SERV:1PPS " + std::to_string(offset));
    comm->eat();
  }

  /* @brief Function to set the frequency at which a debug trace is produced
   *
   * Format:
   *   <date> <1PPS count> <fine DAC> <UTC offset (ns)> <freq error estimate>
   *   <visible SV's> <tracked SV's> <lock state> <health status>
   *
   * Note: Jackson Labs FW version 0.913+ is required to use this command
   *
   * @param The frequency at which a debug trace should be produced
   */
  void serv_trac(size_t freq)
  {
    comm->write("SERV:TRAC " + std::to_string(freq));
    comm->eat();
  }
};

} // namespace cyrial

#endif // CYRIAL_DEVICES_GPSDO_HPP
