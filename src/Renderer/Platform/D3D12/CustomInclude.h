#pragma once

#include <d3dcommon.h>

#include <Core/Platform/Windows/WindowsGuGuFile.h>

namespace GuGu {
	class CustomInclude : public ID3DInclude
	{
	public:
		STDMETHOD(Open)(
			D3D_INCLUDE_TYPE IncludeType,
			LPCSTR pFileName,
			LPCVOID pParentData,
			LPCVOID* ppData,
			UINT* pBytes
			) {
			// 在这里实现你的文件读取操作
			// 使用pFileName加载着色器文件并将数据存储在ppData中
			// 设置pBytes为文件大小

			// 示例代码，需要根据实际情况修改
			//*ppData = YourCustomFileReadFunction(pFileName, pBytes);
			GuGuUtf8Str pathStr(pFileName);
			m_file.OpenFile(pathStr, GuGuFile::OnlyRead);
			int32_t fileSize = m_file.getFileSize();
			int32_t haveReadedBytes = 0;
			bool retValue = m_file.ReadFile(ppData, fileSize, haveReadedBytes);
			*pBytes = haveReadedBytes;
			if (retValue) return S_OK;
			else return S_FALSE;
		}

		STDMETHOD(Close)(LPCVOID pData) {
			// 关闭文件，释放资源等操作
			// 示例代码中不需要实际执行，可以留空或者根据需要实现
			m_file.CloseFile();
			return S_OK;
		}
	private:
		WindowsGuGuFile m_file;
	};
}