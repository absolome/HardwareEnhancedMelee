﻿using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Net;
using System.Net.NetworkInformation;
using System.Text;
using System.Threading.Tasks;
using Fizzi.Applications.SlippiConfiguration.Common;
using Newtonsoft.Json.Linq;
using Fizzi.Applications.SlippiConfiguration.Model;

namespace Fizzi.Applications.SlippiConfiguration.ViewModel
{
    class SlippiDevice : INotifyPropertyChanged
    {
        private const int MAX_LOG_LENGTH = 1000;

        public PhysicalAddress Mac { get; private set; }
        public IPAddress Ip { get; private set; }

        private StringBuilder logBuilder = new StringBuilder(0, MAX_LOG_LENGTH);
        public string Log { get { return logBuilder.ToString(); } }

        public SlippiDevice(string mac, IPAddress address)
        {
            Mac = PhysicalAddress.Parse(mac);
            Ip = address;
        }

        public void HandleUdpMessage(byte[] message)
        {
            //Parse JSON
            var json = JObject.Parse(Encoding.ASCII.GetString(message));
            int command = json["type"].Value<int>();

            switch((MessageType)command)
            {
                case MessageType.LogMessage:
                    //Get string and append to log
                    var logMessage = json["message"].Value<string>();
                    AppendLog(logMessage);
                    break;
            }
            
        }

        public void AppendLog(string message)
        {
            //Don't allow stringbuilder to get bigger than maximum length.
            var newLength = logBuilder.Length + message.Length;
            if (newLength > MAX_LOG_LENGTH)
            {
                //Remove characters from the begining of string builder
                var charsToRemove = newLength - MAX_LOG_LENGTH;
                logBuilder.Remove(0, charsToRemove);
            }

            //Append new message to log string
            logBuilder.Append(message);
            this.Raise("Log", PropertyChanged);
        }

        public void EraseFlash()
        {

        }

        public void ChangeTarget(IPAddress address, int port)
        {

        }

        public event PropertyChangedEventHandler PropertyChanged;
    }
}