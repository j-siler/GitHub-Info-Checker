#include "device.hpp"
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include <iostream>
#include <typeinfo>
#include <typeindex>
#include <time.h>
namespace InfoBeamer {

static const QJsonValue::Type
Null(QJsonValue::Null),
Bool(QJsonValue::Bool),
Double(QJsonValue::Double),
String(QJsonValue::String),
Array(QJsonValue::Array),
Object(QJsonValue::Object),
Undefined(QJsonValue::Undefined);

Device::Device(const QJsonObject &obj)
{
    //Get id
    if(!obj.contains("id"))
        throw DeviceException("No id in json", IBErrCode::BAD_JSON);
    if(obj["id"].type()!=Double)
        throw DeviceException("id in json not numeric", IBErrCode::BAD_JSON);
    _id=obj["id"].toInteger();

    //Get description
    if(!obj.contains("description"))
        throw DeviceException("No description in json", IBErrCode::BAD_JSON);
    if(obj["description"].type()!=String)
        throw DeviceException("description in json not String", IBErrCode::BAD_JSON);
    qDebug() << __func__ << __FILE_NAME__ << "line" << __LINE__ << "description in json:"
             << obj["description"].toString();
    _description=obj["description"].toString().toStdString();


    //Get location
    if(!obj.contains("location"))
        throw DeviceException("No location in json", IBErrCode::BAD_JSON);
    if(obj["location"].type()!=String)
        throw DeviceException("location in json not String", IBErrCode::BAD_JSON);
    qDebug() << __func__ << __FILE_NAME__ << "line" << __LINE__ << "location in json:"
             << obj["location"].toString();
    _location=obj["location"].toString().toStdString();

    //Get serial
    if(!obj.contains("serial"))
        throw DeviceException("No serial in json", IBErrCode::BAD_JSON);
    if(obj["serial"].type()!=String)
        throw DeviceException("serial in json not String", IBErrCode::BAD_JSON);
    qDebug() << __func__ << __FILE_NAME__ << "line" << __LINE__ << "serial in json:"
             << obj["serial"].toString();
    _serial=obj["serial"].toString().toStdString();

    //Get status
    if(!obj.contains("status"))
        throw DeviceException("No status in json", IBErrCode::BAD_JSON);

    //    qDebug() << __func__ << __FILE_NAME__ << "line" << __LINE__
    //             << "status in json type:" << obj["status"].type() << "data:" << obj["status"];
    //    std::cerr << "status type=";
    //    switch (obj["status"].type())
    //    {
    //    case Array:
    //        std::cerr << "Array";
    //        break;
    //    case Bool:
    //        std::cerr << "Bool";
    //        break;
    //    case Double:
    //        std::cerr << "Double";
    //        break;
    //    case Null:
    //        std::cerr << "Null";
    //        break;
    //    case Object:
    //        std::cerr << "Object";
    //        break;
    //    case String:
    //        std::cerr << "String";
    //        break;
    //    case Undefined:
    //        std::cerr << "Undefined";
    //        break;
    //    default:
    //        std::cerr << "Illegal type!!!!!";
    //    }
    //    std::cerr << std::endl;


    if(obj["status"].type()==String)
    {
        qDebug() << __func__ << __FILE_NAME__ << "line" << __LINE__ << "status in json:"
                 << obj["status"].toString().toStdString().c_str();
        _status=obj["status"].toString().toStdString();
    }
    else if(obj["status"].type()==Null)
    {
        qDebug() << __func__ << __FILE_NAME__ << "line" << __LINE__ << "status in json is type Null:"
                 << obj["status"].toString().toStdString().c_str();
        _status=obj["status"].toString().toStdString();
    }
    else
    {
        qDebug() << __func__ << __FILE_NAME__ << "line" << __LINE__ << "status in json unexpected type:"
                 << obj["status"].type() << obj["status"];
        throw DeviceException("status in json not String/Null", IBErrCode::BAD_JSON);
    }

    //Get is_online
    if(!obj.contains("is_online"))
    {
        throw DeviceException("No is_online in json", IBErrCode::BAD_JSON);
    }
    {
        const auto &v=obj["is_online"];
        if(v.type()!=Bool)
            throw DeviceException("is_online in json not Bool", IBErrCode::BAD_JSON);
        _is_onLine=v.toBool();
    }

    //Get is_synced
    if (!obj.contains("is_synced"))
        _is_synced=nullptr;
    else
    {
        const auto &v=obj["is_synced"];
        if(v.type()!=Bool)
            throw DeviceException("is_synced in json not Bool", IBErrCode::BAD_JSON);
        _is_synced=new bool(v.toBool());
    }

    //Get maintenance strings (if any)
    if(obj.contains("maintenace"))
    {
        const auto &v=obj["maintenance"];
        if(v.type()==Array)
        {
            for(const auto& m: v.toArray())
            {
                if(m.type()!=String)
                {
                    qDebug() << __func__ << __FILE_NAME__ << "line" << __LINE__
                             << "Expected type() of String but got"
                             << m.type();
                }
                else
                {
                    _maintenance.push_back(m.toString().toStdString());
                }
            }
        }
    }

    //Get Run object
    if(obj.contains("run"))
    {
        const auto &ro=obj["run"];
        if(ro.type()!=Object)
        {
            throw DeviceException("run key does not yield Object", IBErrCode::BAD_JSON);
        }
        const auto &run=ro.toObject();

        //Get channel
        if(!run.contains("channel"))
        {
            throw DeviceException("run object does not contain \"channel\"", IBErrCode::BAD_JSON);
        }
        if(run["channel"].type()!=String)
        {
            throw DeviceException("run object \"channel\" is not String", IBErrCode::BAD_JSON);
        }
        _run.channel=run["channel"].toString().toStdString();

        //Get public_addr
        if(!run.contains("public_addr"))
        {
            throw DeviceException("run object does not contain \"public_addr\"", IBErrCode::BAD_JSON);
        }
        if(run["public_addr"].type()!=String)
        {
            throw DeviceException("run object \"public_addr\" is not String", IBErrCode::BAD_JSON);
        }
        _run.public_addr=run["public_addr"].toString().toStdString();

        //Get resolution
        if(!run.contains("resolution"))
        {
            throw DeviceException("run object does not contain \"resolution\"", IBErrCode::BAD_JSON);
        }
        if(run["resolution"].type()!=String)
        {
            throw DeviceException("run object \"resolution\" is not String", IBErrCode::BAD_JSON);
        }
        _run.resolution=run["resolution"].toString().toStdString();

        //Get restarted
        if(!run.contains("restarted"))
        {
            throw DeviceException("run object does not contain \"restarted\"", IBErrCode::BAD_JSON);
        }
        if(run["restarted"].type()!=Double)
        {
            throw DeviceException("run object \"restarted\" is not Double", IBErrCode::BAD_JSON);
        }
        _run.restarted=time_t(run["resolution"].toInteger());

        //Get tag
        if(!run.contains("tag"))
        {
            throw DeviceException("run object does not contain \"tag\"", IBErrCode::BAD_JSON);
        }
        if(run["tag"].type()!=String)
        {
            throw DeviceException("run object \"tag\" is not String", IBErrCode::BAD_JSON);
        }
        _run.tag=run["tag"].toString().toStdString();

        //Get version
        if(!run.contains("version"))
        {
            throw DeviceException("run object does not contain \"version\"", IBErrCode::BAD_JSON);
        }
        if(run["version"].type()!=String)
        {
            throw DeviceException("run object \"version\" is not String", IBErrCode::BAD_JSON);
        }
        _run.version=run["version"].toString().toStdString();

        //Get pi_revision
        if(!run.contains("pi_revision"))
        {
            throw DeviceException("run object does not contain \"pi_revision\"", IBErrCode::BAD_JSON);
        }
        if(run["pi_revision"].type()!=String)
        {
            throw DeviceException("run object \"pi_revision\" is not String", IBErrCode::BAD_JSON);
        }
        _run.pi_revision=run["pi_revision"].toString().toStdString();
    }

    //Get userdata
    if(obj.contains("userdata"))
        _userdata=new QJsonValue(obj["userdata"]);

    //Get reboot
    if(!obj.contains("reboot"))
    {
        throw DeviceException("json object does not contain \"reboot\"", IBErrCode::BAD_JSON);
    }
    if(obj["reboot"].type()!=Double)
    {
        throw DeviceException("json object \"reboot\" is not Double", IBErrCode::BAD_JSON);
    }
    _reboot=time_t(obj["reboot"].toInteger());

    //Get geo
    if(obj.contains("geo") && obj["geo"].type()!=Null)
    {
        const auto &g=obj["geo"];
        if (g.type()!=Object)
        {
            throw DeviceException("json object \"geo\" is not Object", IBErrCode::BAD_JSON);
        }
        const auto &geo=g.toObject();

        //Get lat
        if(!geo.contains("lat"))
        {
            throw DeviceException("geo object does not contain \"lat\"", IBErrCode::BAD_JSON);
        }
        if(geo["lat"].type()!=Double)
        {
            throw DeviceException("geo object \"lat\" is not Double", IBErrCode::BAD_JSON);
        }
        _geo=new Geo;
        _geo->lat=geo["lat"].toDouble();

        //Get lon
        if(!geo.contains("lon"))
        {
            throw DeviceException("geo object does not contain \"lon\"", IBErrCode::BAD_JSON);
        }
        if(geo["lon"].type()!=Double)
        {
            throw DeviceException("geo object \"lon\" is not Double", IBErrCode::BAD_JSON);
        }
        _geo->lon=geo["lon"].toDouble();

        //Get source
        if(!geo.contains("source"))
        {
            throw DeviceException("geo object does not contain \"source\"", IBErrCode::BAD_JSON);
        }
        if(geo["source"].type()!=String)
        {
            throw DeviceException("geo object \"lon\" is not Double", IBErrCode::BAD_JSON);
        }
        QString source(geo["source"].toString());
        _geo->source=source.toStdString();
        //_geo->source=geo["source"].toString().toStdString();
    }

    //Get setup
    if(obj.contains("setup") && obj["setup"].type()!=Null)
    {
        const auto &s=obj["setup"];
        if(s.type()!=Object)
            throw DeviceException("json object \"setup\" is not Object", IBErrCode::BAD_JSON);
        const auto &setup=s.toObject();

        //Get id
        if(!setup.contains("id"))
            throw DeviceException("setup object does not contain \"id\"", IBErrCode::BAD_JSON);
        if(setup["id"].type()!=Double)
            throw DeviceException("setup object \"id\" not Double", IBErrCode::BAD_JSON);
        _setup=new Setup;
        _setup->id=setup["id"].toInteger();

        //Get name
        if(!setup.contains("name"))
            throw DeviceException("setup object does not contain \"name\"", IBErrCode::BAD_JSON);
        if(setup["name"].type()!=String)
            throw DeviceException("setup object \"name\" not String", IBErrCode::BAD_JSON);
        _setup->name=setup["name"].toString().toStdString();

        //Get updated
        if(!setup.contains("updated"))
            throw DeviceException("setup object does not contain \"updated\"", IBErrCode::BAD_JSON);
        if(setup["updated"].type()!=Double)
            throw DeviceException("setup object \"updated\" not Double", IBErrCode::BAD_JSON);
        _setup->updated=time_t(setup["updated"].toInteger());


    }

    //Get hw
    if(obj.contains("hw") && obj["hw"].type()!=Null)
    {
        _hw=new Hw;
        const auto &h=obj["hw"];
        if(h.type()!=Object)
            throw DeviceException("json object \"hw\" is not Object", IBErrCode::BAD_JSON);
        const auto &hw=h.toObject();

        //Get type
        if(hw.contains("type"))
        {
            if(hw["type"].type()!=String)
                throw DeviceException("hw object \"type\" not String", IBErrCode::BAD_JSON);
            _hw->hw_type=hw["type"].toString().toStdString();
        }

        //Get platform
        if(hw.contains("platform") && hw["platform"].type()!=Null)
        {
            if(hw["platform"].type()!=String)
                throw DeviceException("hw object \"platform\" not String", IBErrCode::BAD_JSON);
            _hw->platform=hw["platform"].toString().toStdString();
        }

        //Get model
        if(hw.contains("model")  && hw["model"].type()!=Null)
        {
            if(hw["model"].type()!=String)
                throw DeviceException("hw object \"model\" not String", IBErrCode::BAD_JSON);
            _hw->model=hw["model"].toString().toStdString();
        }

        //Get memory
        if(hw.contains("memory"))
        {
            if(hw["memory"].type()!=Double)
                throw DeviceException("hw object \"memory\" not Double", IBErrCode::BAD_JSON);
            _hw->memory=time_t(hw["memory"].toInteger());
        }

        //Get features
        if(hw.contains("features") && hw["features"]!=Null)
        {
            if(!hw["features"].isArray())
                throw DeviceException("hw object \"features\" not Array", IBErrCode::BAD_JSON);
            const auto &features=hw["features"].toArray();
            for(const auto &feature: features)
            {
                if (!feature.isString())
                    qDebug() << __func__ << __FILE_NAME__ << "line" << __LINE__
                             << "hw object \"features\" not String";
                _hw->features.push_back(feature.toString().toStdString());
            }
        }
    }

    //Get offline
    if(!obj.contains("offline"))
    {
        throw DeviceException("json object does not contain \"offline\"", IBErrCode::BAD_JSON);
    }
}


void Device::poplulate(const QJsonObject &obj)
{
    Device::devices.clear();
    if(!obj.contains("devices"))
        throw DeviceException("json missing key \"devices\"", IBErrCode::BAD_JSON);
    const QJsonValue &a(obj["devices"]);
    if(a.type()!=Array)
        throw DeviceException("json value \"devices\" is not an array", IBErrCode::BAD_JSON);
    const QJsonArray &da(a.toArray());
    for(int i=0; i<da.size(); i++)
    {
        if(da[i].type()!=Object)
        {
            std::string msg;
            msg+="json \"devices[";
            msg+=std::to_string(i);
            msg+="]\" not QJsonObject";
            throw(DeviceException(msg, IBErrCode::BAD_JSON));
        }
        devices.push_back(Device(da[i].toObject()));
        std::cerr << devices.back();
    }
}

std::vector<Device> Device::devices;

static inline const std::string &typeToString(const QJsonValue &v)
{
    QJsonValue::Type t=v.type();
    static std::string TYPESTRING[]={"Null",
                                     "Bool",
                                     "Double",
                                     "String",
                                     "Array",
                                     "Object",
                                     "Undefined",
                                     "Illegal"};
    size_t index =
            (t>=Null && t<=Object)
            ? t : t==Undefined ? Object+1 : Object+2;
    return TYPESTRING[index];
}



static inline std::ostream& operator << (std::ostream & os, const QJsonValue & v)
{
    os << typeToString(v);
    switch (v.type())
    {
    // Types that do not have a value to display
    case Null:
    case Object:
    case Array:
    case Undefined:
    default:
        break;
        // Types that display a value
    case Bool:
        os << '(' << (v.toBool() ? "true" : "false") << ')';
        break;
    case String:
        os << "(\"" << v.toString().toStdString() << "\")";
        break;
    case Double:
    {
        double vDbl(v.toDouble());
        int64_t vInt(v.toDouble());
        if(vDbl==vInt)
            //Integer
            os << '(' << vInt << ')';
        else
            os << '(' << vDbl << ')';
    }
    }
    return os;
}

std::ostream& operator << (std::ostream& os, const Device &d)
{
    using std::endl;
    os << "id=" << d._id << endl
       << "desciption=\"" << d._description << "\"" << endl
       << "serial=\"" << d._serial << "\"" << endl
       << "status=\"" << d._status << "\"" << endl
       << "is " << (d._is_onLine ? "" : "not ") << "on_line" << endl
       << "is_synced=" << (d._is_synced==nullptr ? "null" : (d._is_synced ? "true" : "false")) << endl
       << "maintenance strings:" << endl;
    //Maintenance strings
    for (size_t i=0; i<d._maintenance.size(); i++)
        os << '\t' << i <<"\t\"" << d._maintenance[i] << '\"' << endl;
    //Run object
    os << "run:" << endl
       << "\tchannel=\"" << d._run.channel << '\"' << endl
       << "\tpublic_addr=\"" << d._run.public_addr << '\"' << endl
       << "\tresolution=\"" << d._run.resolution << '\"' << endl
       << "\trestarted at " << ctime(&d._run.restarted)
       << "\ttag=\"" << d._run.tag << '\"' << endl
       << "\tversion=\"" << d._run.version << '\"' << endl
       << "\tpi_revision=\"" << d._run.pi_revision;

    //Userdata
    if(d._userdata!=nullptr)
        os << "userdata="
           << *(d._userdata)
           << endl;


    os << "\treboot at " << ctime(&d._reboot) << endl;

    //Geo data
    if (d._geo==nullptr)
        os << "geo is Null" << endl;
    else
    {
        os << "geo:" << endl
           << "\tlat=" << d._geo->lat
           << ", lon=" << d._geo->lon
           << ", source=\"" << d._geo->source
           << "\"" << endl;
    }

    //Setup data
    if (d._setup==nullptr)
        os << "setup is Null" << endl;
    else
    {
        os << "setup:" << endl
           << "\tid=" << d._setup->id
           << ", name=\"" << d._setup->name
           << ",\" updated at " << ctime(&d._setup->updated)
           << endl;
    }

    //Hw
    if (d._hw==nullptr)
        os << "setup is Null" <<  endl;
    else
    {
        os << "hw:" << endl
           << "\ttype=\"" << d._hw->hw_type
           << "\", model=\"" << d._hw->model
           << "\", memory=" << d._hw->memory
           << "MB, platform=\"" << d._hw->platform
           << "\"" << endl;
        if(!d._hw->features.empty())
        {
            os << "\features:" << endl;
            for(const auto &f: d._hw->features)
                os << "\t\t\"" << f << "\"" << endl;
        }
    }

    //offline
    os << "offline"
       << "\tlicensed=" << (d._offline.licensed ? "true" : "false")
       << ", plan=\"" << d._offline.plan << "\""
       << ", tmax_offline=" << d._offline.max_offline << " days"
       << ", chargeable=" << d._offline.chargeable << " days" << endl;

    os << "upgrade_blocked=" << d._upgrade_blocked << " days" << endl;
    return os;
}

}
