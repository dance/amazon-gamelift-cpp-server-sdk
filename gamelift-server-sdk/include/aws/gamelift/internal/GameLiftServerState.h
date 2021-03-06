/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/
#pragma once

#include <aws/gamelift/internal/GameLiftCommonState.h>
#include <aws/gamelift/server/GameLiftServerAPI.h>
#include <aws/gamelift/internal/network/Network.h>
#include <condition_variable>

namespace Aws
{
namespace GameLift
{
namespace Internal
{
    using namespace Aws::GameLift::Server::Model;

#ifndef GAMELIFT_USE_STD
    typedef Outcome<GameLiftServerState*, GameLiftError> InitSDKOutcome;
#endif

    class GameLiftServerState : public GameLiftCommonState, public Network::AuxProxyMessageHandler
    {
#ifdef GAMELIFT_USE_STD
    public:

        static Server::InitSDKOutcome CreateInstance();

        virtual GAMELIFT_INTERNAL_STATE_TYPE GetStateType() override { return GAMELIFT_INTERNAL_STATE_TYPE::SERVER; };

        // Singleton constructors should be private, but we are using a custom allocator that needs to be 
        // able to see them. Don't use these.
        GameLiftServerState();

        ~GameLiftServerState();

        GenericOutcome ProcessReady(const Aws::GameLift::Server::ProcessParameters &processParameters);

        GenericOutcome ProcessEnding();

        GenericOutcome InitializeNetworking();

        GenericOutcome ShutdownNetworking();

        GenericOutcome ActivateGameSession();

        GenericOutcome UpdatePlayerSessionCreationPolicy(PlayerSessionCreationPolicy newPlayerSessionPolicy);

        GenericOutcome TerminateGameSession();

        std::string GetGameSessionId();

        GenericOutcome AcceptPlayerSession(const std::string& playerSessionId);

        GenericOutcome RemovePlayerSession(const std::string& playerSessionId);

        DescribePlayerSessionsOutcome DescribePlayerSessions(const Aws::GameLift::Server::Model::DescribePlayerSessionsRequest &describePlayerSessionsRequest);

        bool IsProcessReady() { return m_processReady; }

        //From Network::AuxProxyMessageHandler
        void OnStartGameSession(GameSession& gameSession, sio::message::list& ack_resp) override;
        void OnTerminateProcess() override;

    private:
        bool AssertNetworkInitialized();

        bool CreateSDKDetectionDirectory(std::string dir);

        void WaitForAndPopulateAckResponse(sio::message::list &ack_resp);

        void ReportHealth();
        void HealthCheck();
        bool DefaultHealthCheck() { return true; }

        std::function<void(Aws::GameLift::Server::Model::GameSession)> m_onStartGameSession;
        std::function<void()> m_onProcessTerminate;
        std::function<bool()> m_onHealthCheck;

        volatile bool m_processReady;

        //Only one game session per process.
        std::string m_gameSessionId;

        Aws::GameLift::Internal::Network::Network* m_network;

		std::mutex m_lockHealthCheck;
		std::condition_variable_any m_condHealthCheck;
#else
    public:

        static InitSDKOutcome CreateInstance();

        virtual GAMELIFT_INTERNAL_STATE_TYPE GetStateType() override { return GAMELIFT_INTERNAL_STATE_TYPE::SERVER; };

        // Singleton constructors should be private, but we are using a custom allocator that needs to be 
        // able to see them. Don't use these.
        GameLiftServerState();

        ~GameLiftServerState();

        GenericOutcome ProcessReady(const Aws::GameLift::Server::ProcessParameters &processParameters);

        GenericOutcome ProcessEnding();

        GenericOutcome InitializeNetworking();

		GenericOutcome ShutdownNetworking();

        GenericOutcome ActivateGameSession();

        GenericOutcome UpdatePlayerSessionCreationPolicy(PlayerSessionCreationPolicy newPlayerSessionPolicy);

        GenericOutcome TerminateGameSession();

        std::string GetGameSessionId();

        GenericOutcome AcceptPlayerSession(const std::string& playerSessionId);

        GenericOutcome RemovePlayerSession(const std::string& playerSessionId);

        DescribePlayerSessionsOutcome DescribePlayerSessions(const Aws::GameLift::Server::Model::DescribePlayerSessionsRequest &describePlayerSessionsRequest);

        bool IsProcessReady() { return m_processReady; }

        //From Network::AuxProxyMessageHandler
        void OnStartGameSession(GameSession& gameSession, sio::message::list& ack_resp) override;
        void OnTerminateProcess() override;

    private:
        bool AssertNetworkInitialized();

        bool CreateSDKDetectionDirectory(std::string dir);

        void WaitForAndPopulateAckResponse(sio::message::list &ack_resp);

        void ReportHealth();
        void HealthCheck();
        bool DefaultHealthCheck() { return true; }

        std::function<void(Aws::GameLift::Server::Model::GameSession, void*)> m_onStartGameSession;
        std::function<void(void*)> m_onProcessTerminate;
        std::function<bool(void*)> m_onHealthCheck;

        void* m_startGameSessionState;
        void* m_processTerminateState;
        void* m_healthCheckState;

        void* startGameSessionState;
        void* processTerminateState;
        void* healthCheckState;

		volatile bool m_processReady;

        //Only one game session per process.
        std::string m_gameSessionId;

        Aws::GameLift::Internal::Network::Network* m_network;

		std::mutex m_lockHealthCheck;
		std::condition_variable m_condHealthCheck;
#endif
    };

} // namespace Internal
} // namespace GameLift
} // namespace Aws
