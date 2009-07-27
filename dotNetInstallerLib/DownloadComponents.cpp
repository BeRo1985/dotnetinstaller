#include "StdAfx.h"
#include "DownloadComponents.h"
#include "InstallerLog.h"

DownloadComponents::DownloadComponents()
	: ThreadComponent(undefined)
	, callback(NULL)
{

}

void DownloadComponents::Load(IDownloadCallback * callback_ptr, const std::vector<DownloadComponentInfoPtr>& component_info)
{
	callback = callback_ptr;

	for (size_t i = 0; i < component_info.size(); i++)
	{
		push_back(DownloadComponentPtr(new DownloadComponent(
			callback, component_info[i], i + 1, component_info.size())));
	}
}

int DownloadComponents::ExecOnThread()
{
	try
	{
		for (size_t i = 0; i < size(); i++)
		{
			const DownloadComponentPtr& component = (* this)[i];

			if (callback && callback->IsDownloadCancelled())
				return -2;

			component->Exec();
		}

		if (callback)
		{
			callback->DownloadComplete();
		}
	}
	catch(std::exception& ex)
	{
	    if (callback)
		{
			callback->DownloadError(DVLib::string2wstring(ex.what()).c_str());
		}

		throw ex;
	}	

	return 0;
}

bool DownloadComponents::IsCopyRequired() const
{
	for (size_t i = 0; i < size(); i++)
	{
		bool required = (* this)[i]->IsCopyRequired();
				
		std::wstring destination = (* this)[i]->GetDestinationFileName();
		LOG(L"Destination '" << destination 
			<< L"', exists=" << (DVLib::FileExists(destination) ? L"True" : L"False")
			<< L", copy=" << (required ? L"True" : L"False"));

		if (required)
		{
			return true;
		}
	}

	return false;
}

bool DownloadComponents::IsDownloadRequired() const
{
	for (size_t i = 0; i < size(); i++)
	{
		bool required = (* this)[i]->IsDownloadRequired();
				
		std::wstring destination = (* this)[i]->GetDestinationFileName();
		LOG(L"Destination '" << destination 
			<< L"', exists=" << (DVLib::FileExists(destination) ? L"True" : L"False")
			<< L", download=" << (required ? L"True" : L"False"));

		if (required)
		{
			return true;
		}
	}

	return false;
}
