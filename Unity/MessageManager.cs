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
    public class MessageManager
    {
        static MessageManager mInstance;
        public static MessageManager Instance
        {
            get
            {
                return mInstance ?? (mInstance = new MessageManager());
            }
        }
        Dictionary<string, Action<object[]>> mMessageDict = new Dictionary<string, Action<object[]>>(32);

        // Dictionary to distribute the message cache, mainly to process the situation that the message has not been registered but Dispatch has been called
        Dictionary<string, object[]> mDispatchCacheDict = new Dictionary<string, object[]>(16);

        private MessageManager() { }

        // Subscribe
        public void Subscribe(string message, Action<object[]> action)
        {
            Action<object[]> value = null;

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

        // Dispatch message
        public void Dispatch(string message, object[] args = null, bool addToCache = false)
        {
            if (addToCache)
            {
                mDispatchCacheDict[message] = args;
            }
            else
            {
                Action<object[]> value = null;
                if (mMessageDict.TryGetValue(message, out value))
                {
                    value(args);
                }
            }
        }

        // Disptch message cache
        public void ProcessDispathCache(string message)
        {
            object[] value = null;
            if (mDispatchCacheDict.TryGetValue(message, out value))
            {
                Dispatch(message, value);
                mDispatchCacheDict.Remove(message);
            }
        }
    }
}
