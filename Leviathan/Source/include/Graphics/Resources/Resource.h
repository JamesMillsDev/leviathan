#pragma once

namespace Leviathan
{
	class IResource
	{
	public:
		virtual ~IResource() = default;

	};

	template<typename T>
	class Resource : public IResource
	{
	private:
		T* m_resource;

	public:
		explicit Resource(T* resource) :
			m_resource{ resource }
		{

		}

		~Resource() override
		{
			delete m_resource;
		}

		Resource(const Resource&) = default;
		Resource(Resource&&) noexcept = default;

	public:
		T* Get() const
		{
			return m_resource;
		}

	public:
		Resource& operator=(const Resource&) = default;
		Resource& operator=(Resource&&) noexcept = default;

		T operator*()
		{
			return *m_resource;
		}

		T* operator->()
		{
			return m_resource;
		}

	};
}
