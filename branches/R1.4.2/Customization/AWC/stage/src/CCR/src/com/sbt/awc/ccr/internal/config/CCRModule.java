// @<COPYRIGHT>@
// ==================================================
// Copyright 2016.
// Siemens Product Lifecycle Management Software Inc.
// All Rights Reserved.
// ==================================================
// @<COPYRIGHT>@

package com.sbt.awc.ccr.internal.config;

import com.sbt.awc.ccr.internal.commands.StartCCRCommandHandler;
import com.siemens.splm.clientfx.ui.commands.published.CommandsExtensionPointHelper;
import com.siemens.splm.clientfx.ui.commands.published.CommandId;
import com.gwtplatform.mvp.client.gin.AbstractPresenterModule;
import com.google.inject.Provider;
import com.sbt.awc.ccr.internal.NameTokens;

/**
 * Module CCR.
 */
public class CCRModule
    extends AbstractPresenterModule
{
    @Override
    protected void configure()
    {

        CommandsExtensionPointHelper.registerCommandHandler( binder(), NameTokens.CMD_StartCCR, NameTokens.CMD_StartCCR, StartCCRCommandHandler.class );
    }
    /** Command ID Provider for StartCCR command. */
    public static class StartCCRCommandIdProvider
        implements Provider<CommandId>
     {
        @Override
        public CommandId get()
        {
            return new CommandId( NameTokens.CMD_StartCCR );
        }
    }

}
