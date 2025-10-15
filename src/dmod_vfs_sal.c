/**
 * MIT License
 * 
 * Copyright (c) 2025 Choco-Technologies
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * @brief DMOD VFS - DMOD SAL Implementation
 * 
 * This file provides the implementation of the DMOD file API functions required
 * by the DMOD system. These functions are part of the System Abstraction Layer (SAL)
 * and are expected to be provided by the system side.
 */

#include "dmod_vfs.h"
#include <stddef.h>
#include <stdbool.h>

//==============================================================================
//                              DMOD FILE API IMPLEMENTATION
//==============================================================================

/**
 * @brief Implementation of Dmod_FileOpen for system side
 */
void* Dmod_FileOpen(const char* Path, const char* Mode)
{
    return DmodVfs_FileOpen(Path, Mode);
}

/**
 * @brief Implementation of Dmod_FileRead for system side
 */
size_t Dmod_FileRead(void* Buffer, size_t Size, size_t Count, void* File)
{
    return DmodVfs_FileRead(Buffer, Size, Count, File);
}

/**
 * @brief Implementation of Dmod_FileWrite for system side
 */
size_t Dmod_FileWrite(const void* Buffer, size_t Size, size_t Count, void* File)
{
    return DmodVfs_FileWrite(Buffer, Size, Count, File);
}

/**
 * @brief Implementation of Dmod_FileSeek for system side
 */
int Dmod_FileSeek(void* File, long Offset, int Origin)
{
    return DmodVfs_FileSeek(File, Offset, Origin);
}

/**
 * @brief Implementation of Dmod_FileTell for system side
 */
size_t Dmod_FileTell(void* File)
{
    return DmodVfs_FileTell(File);
}

/**
 * @brief Implementation of Dmod_FileSize for system side
 */
size_t Dmod_FileSize(void* File)
{
    return DmodVfs_FileSize(File);
}

/**
 * @brief Implementation of Dmod_FileClose for system side
 */
void Dmod_FileClose(void* File)
{
    DmodVfs_FileClose(File);
}

/**
 * @brief Implementation of Dmod_FileAvailable for system side
 */
bool Dmod_FileAvailable(const char* Path)
{
    return DmodVfs_FileAvailable(Path);
}
