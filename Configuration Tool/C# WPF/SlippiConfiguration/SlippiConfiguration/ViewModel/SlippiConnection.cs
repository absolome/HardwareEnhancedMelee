﻿using NLog;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Reactive.Linq;
using System.Text;
using System.Threading.Tasks;
using Fizzi.Applications.SlippiConfiguration.Common;

namespace Fizzi.Applications.SlippiConfiguration.ViewModel
{
    class SlippiConnection : IDisposable
    {
        private const int MAX_MESSAGE_LENGTH = 1024;
        private static Logger logger = LogManager.GetCurrentClassLogger();

        public Action TerminateAction { get; set; }
        public GameInformation GameInfo { get; private set; }

        public SlippiConnection(TcpClient client)
        {
            //Make new game info
            GameInfo = new GameInformation();

            //Get stream
            var stream = client.GetStream();

            //Handle receiving messages
            Observable.Start(async () =>
            {
                try
                {
                    while(client.Connected)
                    {
                        //First always start by reading the size of the message
                        var lengthBytes = await stream.ReadExactBytesAsync(4);
                        var messageLength = BitConverter.ToInt32(lengthBytes, 0);

                        //Make sure message length isn't too long
                        if (messageLength > MAX_MESSAGE_LENGTH || messageLength < 0) throw new Exception("Message length was too long or too short. Length: " + messageLength);

                        //Read messageLength bytes
                        var message = await stream.ReadExactBytesAsync(messageLength);

                        //Pass off message to GameInfo
                        GameInfo.ProcessMessage(message);
                    }

                    logger.Info("Client connected flag is no longer true.");
                }
                catch (Exception ex)
                {
                    logger.Info(ex, "Connection threw an exception.");
                }
                finally
                {
                    stream.Close();
                    Dispose();
                }
            }, System.Reactive.Concurrency.TaskPoolScheduler.Default);
        }

        public void Dispose()
        {
            if (TerminateAction != null) TerminateAction();
        }
    }
}
