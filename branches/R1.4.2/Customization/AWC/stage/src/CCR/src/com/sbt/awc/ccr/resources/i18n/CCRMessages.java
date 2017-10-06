// @<COPYRIGHT>@
// ==================================================
// Copyright 2016.
// Siemens Product Lifecycle Management Software Inc.
// All Rights Reserved.
// ==================================================
// @<COPYRIGHT>@

package com.sbt.awc.ccr.resources.i18n;

import com.google.gwt.core.client.GWT;
import com.google.gwt.i18n.client.Messages;

/**
 * Messages for CCR.
 */
public interface CCRMessages
    extends Messages
{
    /** Messages instance */
    CCRMessages INSTANCE = GWT.create( CCRMessages.class );

    /**
     * Provides the translated StartCCR command title.
     * @return translated StartCCR command title
     */
    String StartCCRTitle();

}
