/**
 * Copyright 2009-2013 10gen, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  NOTE:
 *    is_utf8 comes form PECL mongo.
 */

static inline int is_utf8(const char *s, int len)
{
    int i;

    for (i = 0; i < len; i++) {
        if (i + 3 < len && (s[i] & 248) == 240 && (s[i + 1] & 192) == 128 && (s[i + 2] & 192) == 128 && (s[i + 3] & 192) == 128) {
            i += 3;
        } else if (i + 2 < len && (s[i] & 240) == 224 && (s[i + 1] & 192) == 128 && (s[i + 2] & 192) == 128) {
            i += 2;
        } else if (i + 1 < len && (s[i] & 224) == 192 && (s[i + 1] & 192) == 128) {
            i += 1;
        } else if ((s[i] & 128) != 0) {
            return 0;
        }
    }

    return 1;
}
