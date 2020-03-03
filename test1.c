% Config params, overwrite any previous settings from the commandline

config speed 80

config ap_on 0
config ap_open 1

config auto_connect 1
config dns dhcp
config netmask 255.255.255.0
config gw 192.168.12.1
config ntp_server 1.pool.ntp.org


config ssid Sebastien@2.4G
config password 83622365

config mqtt_host 192.168.10.125
config mqtt_user Sebastien
config mqtt_password 83622365
config mqtt_port 1883
config mqtt_ssl 1



%ESP8266 broker function, not used now.

%config broker_port 1883
%config broker_access 3
%config broker_user Sebastien
%config broker_password 83622365



% Now the initialization, this is done once after booting
on init
do
	% Device number
	setvar $device_number = 1

    setvar $timer_toggle_flag = 0

	% @<num> vars are stored in flash and are persistent even after reboot 
	setvar $run = @1 + 1
	setvar @1 = $run
	println "This is boot no "|$run

	% Status of the relay
	setvar $LEDR_status=0
	gpio_out 4 $LEDR_status

	% Command topic
	setvar $command_topic="/home/LEDR/" | $device_number | "/command"

	% Status topic
	setvar $status_topic="/home/LEDR/" | $device_number | "/status"

	publish local $status_topic $LEDR_status retained

	% local subscriptions once in 'init'
	subscribe local $command_topic






% Now the MQTT client init, this is done each time the client connects
on mqttconnect
do
	% remote subscriptions for each connection in 'mqttconnect'
	subscribe remote $command_topic

	publish remote $status_topic $LEDR_status retained

% Now the events, checked whenever something happens

% Is there a remote command?
on topic remote $command_topic
do
	println "Received remote command: " | $this_data

	% republish this locally - this does the action
	publish local $command_topic $this_data





% Is there a local command?
on topic local $command_topic
do
	println "Received local command: " | $this_data

	if $this_data = "on" then
		setvar $LEDR_status = 1
		gpio_out 14 $LEDR_status
	else
	    if $this_data = "off" then
		setvar $LEDR_status = 0
		gpio_out 14 $LEDR_status
	    endif
	endif

	publish local $status_topic $LEDR_status retained
	publish remote $status_topic $LEDR_status retained


% The local pushbutton
on gpio_interrupt 4 pullup
do
	println "New state GPIO 4: " | $this_gpio
	    gpio_out 2 $this_gpio
	endif


% Blinking
on timer 1
do
    %timer reloading
    settimer 1 500

	if $blink = 1 then
		if $timer_toggle_flag = 0 then
            $timer_toggle_flag = 1
        else
            $timer_toggle_flag = 0
        endif
        gpio_out 12 $timer_toggle_flag
	endif
