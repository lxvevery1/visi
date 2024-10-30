#include <iostream>
#include <map>
#include <memory>
#include <sdbus-c++/sdbus-c++.h>

#include "dbus_metadata_picker.h"

int DBusMetadataPicker::dbus_get_audio_name() {
    try {
        // Create a connection to the session bus
        auto connection = sdbus::createSessionBusConnection();

        sdbus::ServiceName destination{"org.mpris.MediaPlayer2.spotify"};
        sdbus::ObjectPath objectPath{"/org/mpris/MediaPlayer2"};

        // Create a proxy for Spotify's MediaPlayer2 interface
        auto proxy = sdbus::createProxy(destination, objectPath);
        if (proxy)
            std::cout << "Proxy created for " << destination << " at "
                      << objectPath << std::endl;

        // Call the Get method on the Metadata property
        sdbus::Variant metadata;
        proxy->callMethod("Get")
            .onInterface("org.freedesktop.DBus.Properties")
            .withArguments("org.mpris.MediaPlayer2.Player", "Metadata")
            .storeResultsTo(metadata);

        // Extract the track name from the metadata
        auto metadataMap =
            metadata.get<std::map<std::string, sdbus::Variant>>();
        auto title = metadataMap.find("xesam:title");
        auto album = metadataMap.find("xesam:album");

        if (title != metadataMap.end()) {
            std::string trackName = title->second.get<std::string>();
            std::string albumName = album->second.get<std::string>();
            std::cout << "Track Name: " << trackName << std::endl;
            std::cout << "Album Name: " << albumName << std::endl;
        } else {
            std::cerr << "Error: Track title not found in Metadata."
                      << std::endl;
        }

    } catch (const sdbus::Error& e) {
        std::cerr << "D-Bus error: [" << e.getName() << "] " << e.getMessage()
                  << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
