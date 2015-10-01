#include "SpecBase.hpp"
#include "Blender/BlenderSupport.hpp"
#include "BlenderConnection.hpp"

namespace Retro
{

static LogVisor::LogModule Log("Retro::SpecBase");

bool SpecBase::canExtract(const ExtractPassInfo& info, std::list<ExtractReport>& reps)
{
    m_disc = NOD::OpenDiscFromImage(info.srcpath.c_str(), m_isWii);
    if (!m_disc)
        return false;
    const char* gameID = m_disc->getHeader().gameID;

    m_standalone = true;
    if (m_isWii && !memcmp(gameID, "R3M", 3))
        m_standalone = false;

    if (m_standalone && !checkStandaloneID(gameID))
        return false;

    char region = m_disc->getHeader().gameID[3];
    static const HECL::SystemString regNONE = _S("");
    static const HECL::SystemString regE = _S("NTSC");
    static const HECL::SystemString regJ = _S("NTSC-J");
    static const HECL::SystemString regP = _S("PAL");
    const HECL::SystemString* regstr = &regNONE;
    switch (region)
    {
    case 'E':
        regstr = &regE;
        break;
    case 'J':
        regstr = &regJ;
        break;
    case 'P':
        regstr = &regP;
        break;
    }

    if (m_standalone)
        return checkFromStandaloneDisc(*m_disc, *regstr, info.extractArgs, reps);
    else
        return checkFromTrilogyDisc(*m_disc, *regstr, info.extractArgs, reps);
}

void SpecBase::doExtract(const ExtractPassInfo& info, FProgress progress)
{
    if (!Blender::BuildMasterShader(m_masterShader))
        Log.report(LogVisor::FatalError, "Unable to build master shader blend");
    if (m_isWii)
    {
        /* Extract update partition for repacking later */
        const HECL::SystemString& target = m_project.getProjectWorkingPath().getAbsolutePath();
        NOD::DiscBase::IPartition* update = m_disc->getUpdatePartition();
        NOD::ExtractionContext ctx = {true, info.force, nullptr};

        if (update)
        {
            atUint64 idx = 0;
            progress(_S("Update Partition"), _S(""), 0, 0.0);
            const atUint64 nodeCount = update->getFSTRoot().rawEnd() - update->getFSTRoot().rawBegin();
            ctx.progressCB = [&](const std::string& name) {
                HECL::SystemStringView nameView(name);
                progress(_S("Update Partition"), nameView.sys_str().c_str(), 0, idx / (float)nodeCount);
                idx++;
            };

            update->getFSTRoot().extractToDirectory(target, ctx);
            progress(_S("Update Partition"), _S(""), 0, 1.0);
        }

        if (!m_standalone)
        {
            progress(_S("Trilogy Files"), _S(""), 1, 0.0);
            NOD::DiscBase::IPartition* data = m_disc->getDataPartition();
            const NOD::DiscBase::IPartition::Node& root = data->getFSTRoot();
            for (const NOD::DiscBase::IPartition::Node& child : root)
                if (child.getKind() == NOD::DiscBase::IPartition::Node::NODE_FILE)
                    child.extractToDirectory(target, ctx);
            progress(_S("Trilogy Files"), _S(""), 1, 1.0);
        }
    }
    extractFromDisc(*m_disc, info.force, progress);
}

bool SpecBase::canCook(const HECL::ProjectPath& path)
{
    if (HECL::IsPathBlend(path))
    {
        HECL::BlenderConnection& conn = HECL::BlenderConnection::SharedConnection();
        if (!conn.openBlend(path.getAbsolutePath()))
            return false;
        if (conn.getBlendType() != HECL::BlenderConnection::TypeNone)
            return true;
    }
    else if (HECL::IsPathPNG(path))
    {
        return true;
    }
    else if (HECL::IsPathYAML(path))
    {
        FILE* fp = HECL::Fopen(path.getAbsolutePath().c_str(), _S("r"));
        bool retval = validateYAMLDNAType(fp);
        fclose(fp);
        return retval;
    }
    return false;
}

void SpecBase::doCook(const HECL::ProjectPath& path, const HECL::ProjectPath& cookedPath)
{
}

bool SpecBase::canPackage(const PackagePassInfo& info)
{
    return false;
}

void SpecBase::gatherDependencies(const PackagePassInfo& info,
                                  std::unordered_set<HECL::ProjectPath>& implicitsOut)
{
}

void SpecBase::doPackage(const PackagePassInfo& info)
{
}

}
