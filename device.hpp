#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <string>
#include <vector>
#include <iostream>

#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include <time.h>

#include "InfoBeamer_API_Types.hpp"

namespace  InfoBeamer{

/*!
 *
 *
*/

/*!
 * \brief The Device class
 */

class Device
{
public:
    static void poplulate(const QJsonObject &obj);
    /*!
     * @brief The RunObject struct
     */
    struct RunObject
    {
        std::string channel;        //! The current active release channel
        std::string public_addr;    //! The public IP address of the device
        std::string resolution;     //! A textural representation of the screen resolution active when the device started
        time_t      restarted;      //! The unix timestamp of the last device reboot
        std::string tag;            //! The major release version of the running operating system. Sortable
        std::string version;        //! The exact version of the running operating system. Sortable within its channel
        std::string boot_version;   //! Obsolete: use 'version' instead
        std::string base_version;   //! Obsolete: use 'version' instead
        std::string pi_revision;     //! The hardware revision of the Pi
        std::vector<std::string> features;  //! Not mentioned in documentation but seen in returned data
    };

    /**
     * @brief The Geo struct
     */
    struct Geo
    {
        double    lat, lon; //! The device latitude and longitude if a geolocation is available.
        /*!
         * \brief source
         * Specifies how the geolocation is generated. Can be either "wifi" if it's based on nearby WiFi networks or "ip"
         * if it's based on the device last known public ip address. See also the device location call.
         */
        std::string source;
    };

    /*!
     * \brief The Setup struct
     */
    struct Setup
    {
        int          id;      //! The id of the installed setup.
        std::string name;    //! The name of the installed setup.

        /*!
         * \brief updated
         * Unix timestamp of when the setup was last changed. Changes include name changes, changing the configuration
         * or setting new userdata.
         */
        time_t      updated;
    };

    /*!
     * \brief The Hw struct
     * Information about this hardware.
     */
    struct Hw
    {
        std::string              hw_type;  //! Hardware type, always 'pi' at the moment.
        std::string              model;    //! Model name.
        int                      memory;   //! Memory in MB
        std::string              platform; //! Platform code
        std::vector<std::string> features; //! Lists the supported features.
    };

    /*!
     * \brief The Offline struct
     * Offline support status. Warning, these fields are still work in progress and might change.
     */
    struct Offline
    {
        bool        licensed;    //! Licensed for offline usage? Can either be using hosted or pi standalone software.
        std::string plan;        //! Active offline plan for this device.
        int         max_offline; //! Number of days offline before this device will turn blank.
        int         chargeable;  //! Number of days offline before usage for this device is free.
    };

private:
    Device(const QJsonObject& obj);

    int                     _id;                //! The numerical device id.
    std::string             _description;       //! The device description as given on the Device page.
    std::string             _location;          //! The device location as given on the Device page.
    std::string             _serial;            //! The hardware serial number of the device.
    std::string             _status;            //! An informal string showing what the device is doing at the moment.
    bool                    _is_onLine;         //! true if the device is online and has recently contacted the info-beamer hosted service.
    bool*                   _is_synced=nullptr; //! Is the device in sync with what is configured on info-beamer hosted?

    /*!
     * \brief _maintenance
     * If the device needs maintenance the problem is marked here. See Maintenance flags.  Information about the current boot up cycle.
     *  This information is gathered when the device contacts the info-beamer hosted service. If there has not been any recent
     *  contact, this object might be empty.
     */
    std::vector<std::string>  _maintenance;

    RunObject                 _run; //! See RunObject struct, above

    QJsonValue                *_userdata=nullptr;          //! User supplied opaque data assigned to this device. You can store any data for a device here.

    /*!
     * \brief _reboot
     * The maintenance hour given in an offset from 0:00 in UTC time. The device might reboot in this hour if there is an important update.
     */
    time_t                  _reboot;

    /*!
     * \brief _geo
     * Geolocation information about this device. Can be null if the device hasn't been seen online yet or a device location isn't available.
     */
    Geo*                    _geo=nullptr;

    Setup*                  _setup;              //! Information about the assigned setup. Is null if no setup assigned yet
    Hw                      *_hw=nullptr;        //! Information about the hardware
    Offline                 _offline;            //! Offline support status. Warning, these fields are still work in progress and might change.
    int                     _upgrade_blocked;    //! Number of days this device will not by subject to automated system upgrades.
    static std::vector<Device> devices;
    friend std::ostream& operator << (std::ostream& os, const Device &d);
};
typedef IBException<class Device> DeviceException;
}
#endif // DEVICE_HPP
