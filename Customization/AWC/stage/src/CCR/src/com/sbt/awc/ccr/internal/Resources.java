// @<COPYRIGHT>@
// ==================================================
// Copyright 2016.
// Siemens Product Lifecycle Management Software Inc.
// All Rights Reserved.
// ==================================================
// @<COPYRIGHT>@

package com.sbt.awc.ccr.internal;

import com.google.gwt.core.client.GWT;
import com.google.gwt.resources.client.ClientBundle;
import com.google.gwt.resources.client.ImageResource;

/**
 * Module resources for CCR
 */
public interface Resources
    extends ClientBundle
{
    /** Resources instance */
    Resources INSTANCE = GWT.create( Resources.class );
    /**
     * Get StartCCR command image
     * @return StartCCR command image
     */
    @Source( "com/sbt/awc/ccr/resources/images/StartCCR.png" )
    ImageResource getStartCCRImage();

}
