// @<COPYRIGHT>@
// ==================================================
// Copyright 2016.
// Siemens Product Lifecycle Management Software Inc.
// All Rights Reserved.
// ==================================================
// @<COPYRIGHT>@

package com.sbt.awc.ccr.internal.config;

import com.google.gwt.core.client.GWT;
import com.google.gwt.inject.client.GinModules;
import com.siemens.splm.clientfx.base.extensions.published.IModuleInjector;
import com.sbt.awc.ccr.internal.config.CCRModule;

/**
 * access functions to injected types.  This is limited to the module level access as 
 * we are in the internal namespace.
 */
@GinModules( CCRModule.class )
public interface ICCRInjector
    extends IModuleInjector
{
    /** Injector instance */
    ICCRInjector INSTANCE = GWT.create( ICCRInjector.class );

}
