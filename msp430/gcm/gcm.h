// Copyright © 2013, 2014, Travis Snoozy
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

void c_gcm_aes_encrypt( uint8_t* iv,
                        uint8_t* ekey,
                        uint8_t* hash_const,
                        uint8_t* tag,
                        uint16_t ptlen,
                        uint8_t* pt
#ifdef GCM_ENABLE_AAD
                        ,
                        uint16_t aadlen,
                        uint8_t* aad
#endif // GCM_ENABLE_AAD
                        );

uint8_t c_gcm_aes_decrypt(  uint8_t* iv,
                            uint8_t* ekey,
                            uint8_t* hash_const,
                            uint8_t taglen,
                            uint8_t* tag,
                            uint16_t ptlen,
                            uint8_t* pt
#ifdef GCM_ENABLE_AAD
                            ,
                            uint16_t aadlen,
                            uint8_t* aad
#endif // GCM_ENABLE_AAD
                        );
// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
