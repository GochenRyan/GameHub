// Copyright 2022 Gaocheng Ran <rangaocheng@outlook.com>

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

using System.Collections;
using System.Collections.Generic;
using System;

namespace Common
{
    public class MessageManager<T> where T: class
    {
        static MessageManager<T> mInstance;
        public static MessageManager<T> Instance
        {
            get
            {
                return mInstance ?? (mInstance = new MessageManager<T>());
            }
        }
        Dictionary<string, Action<T>> mMessageDict = new Dictionary<string, Action<T>>(32);

        // Dictionary to distribute the message cache, mainly to process the situation that the message has not been registered but Dispatch has been called
        Dictionary<string, T> mDispatchCacheDict = new Dictionary<string, T>(16);

        private MessageManager() { }

        // Subscribe
        public void Subscribe(string message, Action<T> action)
        {
            Action<T> value = null;

            if (mMessageDict.TryGetValue(message, out value))
            {
                value += action;
                mMessageDict[message] = value;
            }
            else
            {
                mMessageDict.Add(message, action);
            }
        }

        // Unsubscribe
        public void Unsubscribe(string message)
        {
            mMessageDict.Remove(message);
        }

        public void Unsubscribe(string message, Action<T> action)
        {
            Action<T> value = null;

            if (mMessageDict.TryGetValue(message, out value))
            {
                value -= action;
                mMessageDict[message] = value;

                if (mMessageDict[message] == null)
                    mMessageDict.Remove(message);
            }
        }

        // Dispatch message
        public void Dispatch(string message, T args, bool addToCache = false)
        {
            if (addToCache)
            {
                mDispatchCacheDict[message] = args;
            }
            else
            {
                Action<T> value = null;
                if (mMessageDict.TryGetValue(message, out value))
                {
                    value(args);
                }
            }
        }

        // Disptch message cache
        public void ProcessDispathCache(string message)
        {
            T value = null;
            if (mDispatchCacheDict.TryGetValue(message, out value))
            {
                Dispatch(message, value);
                mDispatchCacheDict.Remove(message);
            }
        }
    }
}
