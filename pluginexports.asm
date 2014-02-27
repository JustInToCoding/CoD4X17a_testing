;===========================================================================
;    Copyright (C) 2010-2013  Ninja and TheKelm of the IceOps-Team

;    This file is part of CoD4X17a-Server source code.

;    CoD4X17a-Server source code is free software: you can redistribute it and/or modify
;    it under the terms of the GNU Affero General Public License as
;    published by the Free Software Foundation, either version 3 of the
;    License, or (at your option) any later version.

;    CoD4X17a-Server source code is distributed in the hope that it will be useful,
;    but WITHOUT ANY WARRANTY; without even the implied warranty of
;    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;    GNU Affero General Public License for more details.

;    You should have received a copy of the GNU Affero General Public License
;    along with this program.  If not, see <http://www.gnu.org/licenses/>
;===========================================================================

%macro pexport 1

	SECTION .text	
		%ifdef Windows32
			EXPORT Plugin_%1
		%endif
		global Plugin_%1
		Plugin_%1: jmp dword [p%1]

	SECTION .rodata	
		extern %1
		p%1 dd %1

%endmacro

%macro ralias 2

	SECTION .text		
		%ifdef Windows32
			EXPORT %1
		%endif
		global %1
		%1: jmp dword [p%2]
		
	SECTION .rodata	
		extern %2
		p%2 dd %2
	
%endmacro


ralias Plugin_Printf, Com_Printf
ralias Plugin_PrintWarning, Com_PrintWarning
ralias Plugin_PrintError, Com_PrintError
ralias Plugin_DPrintf, Com_DPrintf

ralias Plugin_ParseGetToken, Com_ParseGetToken
ralias Plugin_ParseTokenLength, Com_ParseTokenLength
ralias Plugin_ParseReset, Com_ParseReset

pexport Cmd_Argv
pexport Cmd_Argc

pexport Cvar_VariableStringBuffer
pexport Cvar_VariableValue
pexport Cvar_VariableIntegerValue
pexport Cvar_VariableString
pexport Cvar_RegisterString
pexport Cvar_RegisterBool
pexport Cvar_RegisterInt
;pexport Cvar_RegisterEnum
pexport Cvar_RegisterFloat
pexport Cvar_SetInt
pexport Cvar_SetBool
pexport Cvar_SetString
pexport Cvar_SetFloat

pexport FS_SV_FOpenFileRead
pexport FS_SV_FOpenFileWrite
pexport FS_Read
pexport FS_ReadLine
pexport FS_Write
pexport FS_FCloseFile

pexport NET_StringToAdr
ralias Plugin_Milliseconds, Sys_Milliseconds


pexport Scr_AddEntity
pexport Scr_AllocArray
pexport Scr_GetNumParam
pexport Scr_GetInt
pexport Scr_GetFloat
pexport Scr_GetString
pexport Scr_GetEntity
pexport Scr_GetConstString
pexport Scr_GetType
pexport Scr_GetVector
pexport Scr_Error
pexport Scr_ParamError
pexport Scr_ObjectError
pexport Scr_AddInt
pexport Scr_AddFloat
pexport Scr_AddBool
pexport Scr_AddString
pexport Scr_AddUndefined
pexport Scr_AddVector
pexport Scr_AddArray
pexport Scr_MakeArray
pexport Scr_Notify
pexport Scr_NotifyNum
pexport Scr_ExecEntThread
pexport Scr_ExecThread
pexport Scr_FreeThread

pexport G_LogPrintf